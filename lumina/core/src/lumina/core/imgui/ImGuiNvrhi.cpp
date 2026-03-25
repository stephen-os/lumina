#include "ImGuiNvrhi.h"
#include "../Log.h"
#include "../Device.h"
#include "../Profiler.h"

#include <imgui.h>

#include <nvrhi/utils.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef LUMINA_PLATFORM_WINDOWS
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl/client.h>
#pragma comment(lib, "d3dcompiler.lib")
#endif

#include <vector>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <unordered_map>

namespace Lumina::ImGuiNvrhi
{
	// GLSL shader source for Vulkan (compiled with glslc)
	static const char* s_ImGuiVSGLSL = R"(
		#version 450

		layout(set = 0, binding = 0) uniform VertexConstantBuffer {
			mat4 ProjectionMatrix;
		};

		layout(location = 0) in vec2 inPosition;
		layout(location = 1) in vec2 inTexCoord;
		layout(location = 2) in vec4 inColor;

		layout(location = 0) out vec4 outColor;
		layout(location = 1) out vec2 outTexCoord;

		void main() {
			gl_Position = ProjectionMatrix * vec4(inPosition, 0.0, 1.0);
			outColor = inColor;
			outTexCoord = inTexCoord;
		}
	)";

	static const char* s_ImGuiPSGLSL = R"(
		#version 450

		layout(set = 0, binding = 1) uniform texture2D fontTexture;
		layout(set = 0, binding = 2) uniform sampler fontSampler;

		layout(location = 0) in vec4 inColor;
		layout(location = 1) in vec2 inTexCoord;

		layout(location = 0) out vec4 outColor;

		void main() {
			outColor = inColor * texture(sampler2D(fontTexture, fontSampler), inTexCoord);
		}
	)";

	// HLSL shader source for D3D12
	static const char* s_ImGuiVSHLSL = R"(
		cbuffer VertexConstantBuffer : register(b0)
		{
			float4x4 ProjectionMatrix;
		};

		struct VS_INPUT
		{
			float2 pos : POSITION;
			float2 uv  : TEXCOORD0;
			float4 col : COLOR0;
		};

		struct PS_INPUT
		{
			float4 pos : SV_POSITION;
			float4 col : COLOR0;
			float2 uv  : TEXCOORD0;
		};

		PS_INPUT main(VS_INPUT input)
		{
			PS_INPUT output;
			output.pos = mul(ProjectionMatrix, float4(input.pos.xy, 0.0f, 1.0f));
			output.col = input.col;
			output.uv  = input.uv;
			return output;
		}
	)";

	static const char* s_ImGuiPSHLSL = R"(
		Texture2D texture0 : register(t0);
		SamplerState sampler0 : register(s0);

		struct PS_INPUT
		{
			float4 pos : SV_POSITION;
			float4 col : COLOR0;
			float2 uv  : TEXCOORD0;
		};

		float4 main(PS_INPUT input) : SV_Target
		{
			float4 out_col = input.col * texture0.Sample(sampler0, input.uv);
			return out_col;
		}
	)";

	// Combined HLSL for D3D12 (uses D3DCompile which needs entry points specified)
	static const char* s_ImGuiShaderHLSLD3D12 = R"(
		cbuffer VertexConstantBuffer : register(b0)
		{
			float4x4 ProjectionMatrix;
		};

		struct VS_INPUT
		{
			float2 pos : POSITION;
			float2 uv  : TEXCOORD0;
			float4 col : COLOR0;
		};

		struct PS_INPUT
		{
			float4 pos : SV_POSITION;
			float4 col : COLOR0;
			float2 uv  : TEXCOORD0;
		};

		PS_INPUT VSMain(VS_INPUT input)
		{
			PS_INPUT output;
			output.pos = mul(ProjectionMatrix, float4(input.pos.xy, 0.0f, 1.0f));
			output.col = input.col;
			output.uv  = input.uv;
			return output;
		}

		SamplerState sampler0 : register(s0);
		Texture2D texture0 : register(t0);

		float4 PSMain(PS_INPUT input) : SV_Target
		{
			float4 out_col = input.col * texture0.Sample(sampler0, input.uv);
			return out_col;
		}
	)";

	static nvrhi::GraphicsAPI s_GraphicsAPI = nvrhi::GraphicsAPI::D3D12;

#ifdef LUMINA_PLATFORM_WINDOWS
	// Helper to execute a process safely without invoking the shell
	static bool ExecuteProcess(const std::string& command, const std::string& args, DWORD& exitCode)
	{
		STARTUPINFOA si = {};
		si.cb = sizeof(si);
		si.dwFlags = STARTF_USESHOWWINDOW;
		si.wShowWindow = SW_HIDE;

		PROCESS_INFORMATION pi = {};

		// CreateProcess requires a mutable command line buffer
		std::string cmdLine = command + " " + args;
		std::vector<char> cmdBuffer(cmdLine.begin(), cmdLine.end());
		cmdBuffer.push_back('\0');

		BOOL success = CreateProcessA(
			nullptr,           // Application name (null = use command line)
			cmdBuffer.data(),  // Command line (mutable)
			nullptr,           // Process security attributes
			nullptr,           // Thread security attributes
			FALSE,             // Inherit handles
			CREATE_NO_WINDOW,  // Creation flags
			nullptr,           // Environment (inherit)
			nullptr,           // Current directory (inherit)
			&si,
			&pi
		);

		if (!success)
		{
			LUMINA_LOG_ERROR("Failed to create process: {}", GetLastError());
			return false;
		}

		// Wait for the process to complete
		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &exitCode);

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		return true;
	}

	// Helper to compile GLSL to SPIR-V using glslc (Vulkan SDK)
	static bool CompileGLSLToSPIRV(const char* source, const char* shaderType, std::vector<uint8_t>& spirvOut)
	{
		// Create temporary files for input and output
		std::filesystem::path tempDir = std::filesystem::temp_directory_path();
		std::filesystem::path inputPath = tempDir / ("imgui_shader_" + std::string(shaderType) + ".glsl");
		std::filesystem::path outputPath = tempDir / ("imgui_shader_" + std::string(shaderType) + ".spv");

		// Write source to temporary file
		{
			std::ofstream inputFile(inputPath, std::ios::binary);
			if (!inputFile)
			{
				LUMINA_LOG_ERROR("Failed to create temporary shader file: {}", inputPath.string());
				return false;
			}
			inputFile.write(source, strlen(source));
		}

		// Build glslc arguments
		std::string stage = (strcmp(shaderType, "vert") == 0) ? "vertex" : "fragment";
		std::string args = "-fshader-stage=" + stage + " \"" + inputPath.string() + "\" -o \"" + outputPath.string() + "\"";

		LUMINA_LOG_INFO("Compiling {} shader with glslc...", stage);

		// Execute glslc using CreateProcess (safer than system())
		DWORD exitCode = 0;
		bool processOk = ExecuteProcess("glslc.exe", args, exitCode);

		std::filesystem::remove(inputPath);

		if (!processOk || exitCode != 0)
		{
			LUMINA_LOG_ERROR("glslc compilation failed with exit code: {}", exitCode);
			return false;
		}

		// Read compiled SPIR-V
		std::ifstream spirvFile(outputPath, std::ios::binary | std::ios::ate);
		if (!spirvFile)
		{
			LUMINA_LOG_ERROR("Failed to read compiled SPIR-V from: {}", outputPath.string());
			return false;
		}

		size_t fileSize = spirvFile.tellg();
		spirvFile.seekg(0);

		spirvOut.resize(fileSize);
		spirvFile.read(reinterpret_cast<char*>(spirvOut.data()), fileSize);
		spirvFile.close();

		std::filesystem::remove(outputPath);

		LUMINA_LOG_INFO("Shader compiled successfully ({} bytes)", fileSize);
		return true;
	}
#endif

	// Renderer state
	static nvrhi::DeviceHandle s_Device;
	static nvrhi::ShaderHandle s_VertexShader;
	static nvrhi::ShaderHandle s_PixelShader;
	static nvrhi::InputLayoutHandle s_InputLayout;
	static nvrhi::BindingLayoutHandle s_BindingLayout;
	static nvrhi::GraphicsPipelineHandle s_Pipeline;
	static nvrhi::BufferHandle s_VertexBuffer;
	static nvrhi::BufferHandle s_IndexBuffer;
	static nvrhi::BufferHandle s_ConstantBuffer;
	static nvrhi::TextureHandle s_FontTexture;
	static nvrhi::SamplerHandle s_FontSampler;
	static nvrhi::BindingSetHandle s_BindingSet;

	// Cache for texture binding sets (texture ptr -> binding set)
	static std::unordered_map<nvrhi::ITexture*, nvrhi::BindingSetHandle> s_TextureBindingSets;

	static uint32_t s_VertexBufferSize = 0;
	static uint32_t s_IndexBufferSize = 0;
	static nvrhi::Format s_RenderTargetFormat = nvrhi::Format::RGBA8_UNORM;

	static bool CreateFontTexture()
	{
		ImGuiIO& io = ImGui::GetIO();

		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		nvrhi::TextureDesc textureDesc;
		textureDesc.dimension = nvrhi::TextureDimension::Texture2D;
		textureDesc.width = static_cast<uint32_t>(width);
		textureDesc.height = static_cast<uint32_t>(height);
		textureDesc.format = nvrhi::Format::RGBA8_UNORM;
		textureDesc.debugName = "ImGui Font Texture";
		textureDesc.initialState = nvrhi::ResourceStates::ShaderResource;
		textureDesc.keepInitialState = true;

		s_FontTexture = s_Device->createTexture(textureDesc);
		if (!s_FontTexture)
		{
			LUMINA_LOG_ERROR("Failed to create ImGui font texture");
			return false;
		}

		// Upload font texture data
		nvrhi::CommandListHandle cmd = s_Device->createCommandList();
		cmd->open();
		cmd->writeTexture(s_FontTexture, 0, 0, pixels, width * 4);
		cmd->close();
		s_Device->executeCommandList(cmd);

		// Store texture ID for ImGui (cast pointer to ImTextureID via uintptr_t for 32/64-bit safety)
		io.Fonts->SetTexID((ImTextureID)(uintptr_t)s_FontTexture.Get());

		return true;
	}

	static bool CreateShaders()
	{
		if (s_GraphicsAPI == nvrhi::GraphicsAPI::VULKAN)
		{
#ifdef LUMINA_PLATFORM_WINDOWS
			// Use glslc to compile GLSL to SPIR-V for Vulkan
			std::vector<uint8_t> vsSPIRV;
			if (!CompileGLSLToSPIRV(s_ImGuiVSGLSL, "vert", vsSPIRV))
			{
				LUMINA_LOG_ERROR("Failed to compile ImGui vertex shader to SPIR-V");
				return false;
			}

			nvrhi::ShaderDesc vsDesc;
			vsDesc.shaderType = nvrhi::ShaderType::Vertex;
			vsDesc.debugName = "ImGui VS";

			s_VertexShader = s_Device->createShader(vsDesc, vsSPIRV.data(), vsSPIRV.size());
			if (!s_VertexShader)
			{
				LUMINA_LOG_ERROR("Failed to create ImGui vertex shader (SPIR-V)");
				return false;
			}

			std::vector<uint8_t> psSPIRV;
			if (!CompileGLSLToSPIRV(s_ImGuiPSGLSL, "frag", psSPIRV))
			{
				LUMINA_LOG_ERROR("Failed to compile ImGui pixel shader to SPIR-V");
				return false;
			}

			nvrhi::ShaderDesc psDesc;
			psDesc.shaderType = nvrhi::ShaderType::Pixel;
			psDesc.debugName = "ImGui PS";

			s_PixelShader = s_Device->createShader(psDesc, psSPIRV.data(), psSPIRV.size());
			if (!s_PixelShader)
			{
				LUMINA_LOG_ERROR("Failed to create ImGui pixel shader (SPIR-V)");
				return false;
			}

			return true;
#else
			LUMINA_LOG_ERROR("Vulkan shader compilation not supported on this platform");
			return false;
#endif
		}

#ifdef LUMINA_PLATFORM_WINDOWS
		// Compile HLSL shaders for D3D12
		ID3DBlob* vsBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		HRESULT hr = D3DCompile(
			s_ImGuiShaderHLSLD3D12,
			strlen(s_ImGuiShaderHLSLD3D12),
			"imgui_vs",
			nullptr,
			nullptr,
			"VSMain",
			"vs_5_0",
			D3DCOMPILE_OPTIMIZATION_LEVEL3,
			0,
			&vsBlob,
			&errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				LUMINA_LOG_ERROR("Failed to compile ImGui vertex shader: {}", (char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}
			return false;
		}

		nvrhi::ShaderDesc vsDesc;
		vsDesc.shaderType = nvrhi::ShaderType::Vertex;
		vsDesc.debugName = "ImGui VS";

		s_VertexShader = s_Device->createShader(vsDesc, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize());
		vsBlob->Release();

		if (!s_VertexShader)
		{
			LUMINA_LOG_ERROR("Failed to create ImGui vertex shader");
			return false;
		}

		// Compile pixel shader
		ID3DBlob* psBlob = nullptr;

		hr = D3DCompile(
			s_ImGuiShaderHLSLD3D12,
			strlen(s_ImGuiShaderHLSLD3D12),
			"imgui_ps",
			nullptr,
			nullptr,
			"PSMain",
			"ps_5_0",
			D3DCOMPILE_OPTIMIZATION_LEVEL3,
			0,
			&psBlob,
			&errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				LUMINA_LOG_ERROR("Failed to compile ImGui pixel shader: {}", (char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}
			return false;
		}

		nvrhi::ShaderDesc psDesc;
		psDesc.shaderType = nvrhi::ShaderType::Pixel;
		psDesc.debugName = "ImGui PS";

		s_PixelShader = s_Device->createShader(psDesc, psBlob->GetBufferPointer(), psBlob->GetBufferSize());
		psBlob->Release();

		if (!s_PixelShader)
		{
			LUMINA_LOG_ERROR("Failed to create ImGui pixel shader");
			return false;
		}

		return true;
#else
		LUMINA_LOG_ERROR("ImGui NVRHI shader compilation not yet implemented for this platform");
		return false;
#endif
	}

	static bool CreatePipeline()
	{
		// Create input layout
		nvrhi::VertexAttributeDesc vertexAttributes[] = {
			nvrhi::VertexAttributeDesc()
				.setName("POSITION")
				.setFormat(nvrhi::Format::RG32_FLOAT)
				.setOffset(offsetof(ImDrawVert, pos))
				.setElementStride(sizeof(ImDrawVert)),
			nvrhi::VertexAttributeDesc()
				.setName("TEXCOORD")
				.setFormat(nvrhi::Format::RG32_FLOAT)
				.setOffset(offsetof(ImDrawVert, uv))
				.setElementStride(sizeof(ImDrawVert)),
			nvrhi::VertexAttributeDesc()
				.setName("COLOR")
				.setFormat(nvrhi::Format::RGBA8_UNORM)
				.setOffset(offsetof(ImDrawVert, col))
				.setElementStride(sizeof(ImDrawVert)),
		};

		s_InputLayout = s_Device->createInputLayout(vertexAttributes, 3, s_VertexShader);
		if (!s_InputLayout)
		{
			LUMINA_LOG_ERROR("Failed to create ImGui input layout");
			return false;
		}

		// Create binding layout
		nvrhi::BindingLayoutDesc bindingLayoutDesc;
		bindingLayoutDesc.visibility = nvrhi::ShaderType::All;

		// For Vulkan, set custom binding offsets so resources map to consecutive bindings
		// Shader expects: binding 0 = constant buffer, binding 1 = texture, binding 2 = sampler
		if (s_GraphicsAPI == nvrhi::GraphicsAPI::VULKAN)
		{
			nvrhi::VulkanBindingOffsets offsets;
			offsets.setConstantBufferOffset(0);   // cb(0) -> binding 0
			offsets.setShaderResourceOffset(1);   // t(0) -> binding 1
			offsets.setSamplerOffset(2);          // s(0) -> binding 2
			bindingLayoutDesc.setBindingOffsets(offsets);
		}

		bindingLayoutDesc.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
		bindingLayoutDesc.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
		bindingLayoutDesc.addItem(nvrhi::BindingLayoutItem::Sampler(0));

		s_BindingLayout = s_Device->createBindingLayout(bindingLayoutDesc);
		if (!s_BindingLayout)
		{
			LUMINA_LOG_ERROR("Failed to create ImGui binding layout");
			return false;
		}

		// Create constant buffer (non-volatile since we only write once per frame)
		nvrhi::BufferDesc cbDesc;
		cbDesc.byteSize = sizeof(float) * 16;
		cbDesc.isConstantBuffer = true;
		cbDesc.isVolatile = false;
		cbDesc.debugName = "ImGui Constant Buffer";
		cbDesc.initialState = nvrhi::ResourceStates::ConstantBuffer;
		cbDesc.keepInitialState = true;

		s_ConstantBuffer = s_Device->createBuffer(cbDesc);
		if (!s_ConstantBuffer)
		{
			LUMINA_LOG_ERROR("Failed to create ImGui constant buffer");
			return false;
		}

		// Create sampler
		nvrhi::SamplerDesc samplerDesc;
		samplerDesc.setAllFilters(true);
		samplerDesc.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap);

		s_FontSampler = s_Device->createSampler(samplerDesc);
		if (!s_FontSampler)
		{
			LUMINA_LOG_ERROR("Failed to create ImGui sampler");
			return false;
		}

		// Create binding set
		nvrhi::BindingSetDesc bindingSetDesc;
		bindingSetDesc.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, s_ConstantBuffer));
		bindingSetDesc.addItem(nvrhi::BindingSetItem::Texture_SRV(0, s_FontTexture));
		bindingSetDesc.addItem(nvrhi::BindingSetItem::Sampler(0, s_FontSampler));

		s_BindingSet = s_Device->createBindingSet(bindingSetDesc, s_BindingLayout);
		if (!s_BindingSet)
		{
			LUMINA_LOG_ERROR("Failed to create ImGui binding set");
			return false;
		}

		// Create graphics pipeline
		nvrhi::GraphicsPipelineDesc pipelineDesc;
		pipelineDesc.inputLayout = s_InputLayout;
		pipelineDesc.VS = s_VertexShader;
		pipelineDesc.PS = s_PixelShader;
		pipelineDesc.primType = nvrhi::PrimitiveType::TriangleList;
		pipelineDesc.addBindingLayout(s_BindingLayout);

		// Blending
		pipelineDesc.renderState.blendState.targets[0].blendEnable = true;
		pipelineDesc.renderState.blendState.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
		pipelineDesc.renderState.blendState.targets[0].destBlend = nvrhi::BlendFactor::InvSrcAlpha;
		pipelineDesc.renderState.blendState.targets[0].blendOp = nvrhi::BlendOp::Add;
		pipelineDesc.renderState.blendState.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
		pipelineDesc.renderState.blendState.targets[0].destBlendAlpha = nvrhi::BlendFactor::InvSrcAlpha;
		pipelineDesc.renderState.blendState.targets[0].blendOpAlpha = nvrhi::BlendOp::Add;

		// Rasterizer
		pipelineDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
		pipelineDesc.renderState.rasterState.fillMode = nvrhi::RasterFillMode::Solid;
		pipelineDesc.renderState.rasterState.scissorEnable = true;

		// Depth
		pipelineDesc.renderState.depthStencilState.depthTestEnable = false;
		pipelineDesc.renderState.depthStencilState.depthWriteEnable = false;

		nvrhi::FramebufferInfo fbInfo;
		fbInfo.addColorFormat(s_RenderTargetFormat);

		s_Pipeline = s_Device->createGraphicsPipeline(pipelineDesc, fbInfo);
		if (!s_Pipeline)
		{
			LUMINA_LOG_ERROR("Failed to create ImGui graphics pipeline");
			return false;
		}

		return true;
	}

	static nvrhi::BindingSetHandle GetOrCreateTextureBinding(nvrhi::ITexture* texture)
	{
		// Check cache first
		auto it = s_TextureBindingSets.find(texture);
		if (it != s_TextureBindingSets.end())
			return it->second;

		// Create new binding set for this texture
		nvrhi::BindingSetDesc bindingSetDesc;
		bindingSetDesc.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, s_ConstantBuffer));
		bindingSetDesc.addItem(nvrhi::BindingSetItem::Texture_SRV(0, texture));
		bindingSetDesc.addItem(nvrhi::BindingSetItem::Sampler(0, s_FontSampler));

		nvrhi::BindingSetHandle bindingSet = s_Device->createBindingSet(bindingSetDesc, s_BindingLayout);
		if (bindingSet)
		{
			s_TextureBindingSets[texture] = bindingSet;
		}
		return bindingSet;
	}

	static bool CreateBuffers(uint32_t vertexCount, uint32_t indexCount)
	{
		// Create or resize vertex buffer
		if (!s_VertexBuffer || s_VertexBufferSize < vertexCount)
		{
			s_VertexBufferSize = vertexCount + 5000;

			nvrhi::BufferDesc vbDesc;
			vbDesc.byteSize = s_VertexBufferSize * sizeof(ImDrawVert);
			vbDesc.isVertexBuffer = true;
			vbDesc.debugName = "ImGui Vertex Buffer";
			vbDesc.initialState = nvrhi::ResourceStates::CopyDest;
			vbDesc.keepInitialState = true;

			s_VertexBuffer = s_Device->createBuffer(vbDesc);
			if (!s_VertexBuffer)
			{
				LUMINA_LOG_ERROR("Failed to create ImGui vertex buffer");
				return false;
			}
		}

		// Create or resize index buffer
		if (!s_IndexBuffer || s_IndexBufferSize < indexCount)
		{
			s_IndexBufferSize = indexCount + 10000;

			nvrhi::BufferDesc ibDesc;
			ibDesc.byteSize = s_IndexBufferSize * sizeof(ImDrawIdx);
			ibDesc.isIndexBuffer = true;
			ibDesc.debugName = "ImGui Index Buffer";
			ibDesc.initialState = nvrhi::ResourceStates::CopyDest;
			ibDesc.keepInitialState = true;

			s_IndexBuffer = s_Device->createBuffer(ibDesc);
			if (!s_IndexBuffer)
			{
				LUMINA_LOG_ERROR("Failed to create ImGui index buffer");
				return false;
			}
		}

		return true;
	}

	bool Init(const ImGuiNvrhiConfig& config)
	{
		s_Device = config.Device;
		s_RenderTargetFormat = config.RenderTargetFormat;
		s_VertexBufferSize = config.InitialVertexBufferSize;
		s_IndexBufferSize = config.InitialIndexBufferSize;
		s_GraphicsAPI = s_Device->getGraphicsAPI();

		if (!CreateShaders())
		{
			LUMINA_LOG_ERROR("Failed to create ImGui shaders");
			return false;
		}

		if (!CreateFontTexture())
		{
			LUMINA_LOG_ERROR("Failed to create ImGui font texture");
			return false;
		}

		if (!CreatePipeline())
		{
			LUMINA_LOG_ERROR("Failed to create ImGui pipeline");
			return false;
		}

		LUMINA_LOG_INFO("ImGui NVRHI backend initialized");
		return true;
	}

	void Shutdown()
	{
		s_TextureBindingSets.clear();
		s_BindingSet = nullptr;
		s_FontSampler = nullptr;
		s_FontTexture = nullptr;
		s_ConstantBuffer = nullptr;
		s_IndexBuffer = nullptr;
		s_VertexBuffer = nullptr;
		s_Pipeline = nullptr;
		s_BindingLayout = nullptr;
		s_InputLayout = nullptr;
		s_PixelShader = nullptr;
		s_VertexShader = nullptr;
		s_Device = nullptr;

		ImGui::GetIO().Fonts->SetTexID(nullptr);

		LUMINA_LOG_INFO("ImGui NVRHI backend shutdown");
	}

	void NewFrame()
	{
		// Nothing to do here for NVRHI backend
	}

	void RenderDrawData(nvrhi::ICommandList* commandList, nvrhi::IFramebuffer* framebuffer, ImDrawData* drawData)
	{
		LUMINA_PROFILE_SCOPE_NC("ImGui::RenderDrawData", Profiler::Colors::UI);

		if (!drawData || drawData->TotalVtxCount == 0)
			return;

		// Create/resize buffers if needed
		if (!CreateBuffers(static_cast<uint32_t>(drawData->TotalVtxCount), static_cast<uint32_t>(drawData->TotalIdxCount)))
			return;

		// Upload vertex/index data
		ImDrawVert* vtxDst = nullptr;
		ImDrawIdx* idxDst = nullptr;

		std::vector<ImDrawVert> vertexData(drawData->TotalVtxCount);
		std::vector<ImDrawIdx> indexData(drawData->TotalIdxCount);

		vtxDst = vertexData.data();
		idxDst = indexData.data();

		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* cmdList = drawData->CmdLists[n];
			memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
			vtxDst += cmdList->VtxBuffer.Size;
			idxDst += cmdList->IdxBuffer.Size;
		}

		commandList->writeBuffer(s_VertexBuffer, vertexData.data(), vertexData.size() * sizeof(ImDrawVert));
		commandList->writeBuffer(s_IndexBuffer, indexData.data(), indexData.size() * sizeof(ImDrawIdx));

		// Setup orthographic projection matrix
		float L = drawData->DisplayPos.x;
		float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
		float T = drawData->DisplayPos.y;
		float B = drawData->DisplayPos.y + drawData->DisplaySize.y;

		float mvp[4][4] = {
			{ 2.0f / (R - L),     0.0f,              0.0f, 0.0f },
			{ 0.0f,               2.0f / (T - B),    0.0f, 0.0f },
			{ 0.0f,               0.0f,              0.5f, 0.0f },
			{ (R + L) / (L - R),  (T + B) / (B - T), 0.5f, 1.0f },
		};

		commandList->writeBuffer(s_ConstantBuffer, mvp, sizeof(mvp));

		// Setup viewport
		nvrhi::Viewport viewport;
		viewport.minX = 0;
		viewport.minY = 0;
		viewport.maxX = drawData->DisplaySize.x;
		viewport.maxY = drawData->DisplaySize.y;
		viewport.minZ = 0.0f;
		viewport.maxZ = 1.0f;

		// Set graphics state
		nvrhi::GraphicsState state;
		state.pipeline = s_Pipeline;
		state.framebuffer = framebuffer;
		state.viewport.addViewport(viewport);
		state.viewport.addScissorRect(nvrhi::Rect(0, static_cast<int>(drawData->DisplaySize.x), 0, static_cast<int>(drawData->DisplaySize.y)));
		state.addBindingSet(s_BindingSet);
		state.addVertexBuffer({ s_VertexBuffer, 0, 0 });
		state.indexBuffer = { s_IndexBuffer, nvrhi::Format::R16_UINT, 0 };

		// Track current texture to minimize binding set changes
		nvrhi::ITexture* currentTexture = s_FontTexture.Get();

		commandList->setGraphicsState(state);

		// Render draw lists
		ImVec2 clipOff = drawData->DisplayPos;
		int globalVtxOffset = 0;
		int globalIdxOffset = 0;

		for (int n = 0; n < drawData->CmdListsCount; n++)
		{
			const ImDrawList* cmdList = drawData->CmdLists[n];

			for (int cmdI = 0; cmdI < cmdList->CmdBuffer.Size; cmdI++)
			{
				const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmdI];

				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(cmdList, pcmd);
				}
				else
				{
					// Apply scissor rect
					ImVec2 clipMin(pcmd->ClipRect.x - clipOff.x, pcmd->ClipRect.y - clipOff.y);
					ImVec2 clipMax(pcmd->ClipRect.z - clipOff.x, pcmd->ClipRect.w - clipOff.y);

					if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
						continue;

					nvrhi::Rect scissor;
					scissor.minX = static_cast<int>(clipMin.x);
					scissor.minY = static_cast<int>(clipMin.y);
					scissor.maxX = static_cast<int>(clipMax.x);
					scissor.maxY = static_cast<int>(clipMax.y);

					state.viewport.scissorRects[0] = scissor;

					// Handle custom textures (cast ImTextureID back to pointer via uintptr_t)
					nvrhi::ITexture* texture = (nvrhi::ITexture*)(uintptr_t)pcmd->GetTexID();
					if (texture != currentTexture)
					{
						currentTexture = texture;
						nvrhi::BindingSetHandle bindingSet = GetOrCreateTextureBinding(texture);
						if (bindingSet)
						{
							state.bindings[0] = { bindingSet };
						}
					}

					commandList->setGraphicsState(state);

					// Draw
					nvrhi::DrawArguments args;
					args.vertexCount = pcmd->ElemCount;
					args.startIndexLocation = pcmd->IdxOffset + globalIdxOffset;
					args.startVertexLocation = pcmd->VtxOffset + globalVtxOffset;

					commandList->drawIndexed(args);
				}
			}

			globalIdxOffset += cmdList->IdxBuffer.Size;
			globalVtxOffset += cmdList->VtxBuffer.Size;
		}
	}

	// =========================================================================
	// Multi-viewport support
	// =========================================================================

	// Native handles cached from graphics_device
	static VulkanNativeHandles s_VkHandles;
#ifdef LUMINA_PLATFORM_WINDOWS
	static D3D12NativeHandles s_DxHandles;
#endif

	// Per-viewport renderer data stored in ImGuiViewport::RendererUserData
	struct ViewportData
	{
		nvrhi::CommandListHandle CommandList;
		std::vector<nvrhi::TextureHandle> SwapchainTextures;
		std::vector<nvrhi::FramebufferHandle> Framebuffers;
		uint32_t Width = 0;
		uint32_t Height = 0;
		uint32_t BackbufferCount = 2;
		uint32_t FrameIndex = 0;
		uint32_t ImageIndex = 0;

		// Vulkan-specific
		VkSurfaceKHR VkSurface = VK_NULL_HANDLE;
		VkSwapchainKHR VkSwapchain = VK_NULL_HANDLE;
		VkFormat VkFormat = VK_FORMAT_UNDEFINED;
		std::vector<VkImage> VkImages;
		std::vector<VkFence> VkFences;

		// D3D12-specific
#ifdef LUMINA_PLATFORM_WINDOWS
		Microsoft::WRL::ComPtr<IDXGISwapChain4> DxSwapchain;
		Microsoft::WRL::ComPtr<ID3D12Fence> DxFence;
		std::vector<uint64_t> DxFenceValues;
		uint64_t DxCurrentFenceValue = 1;
		HANDLE DxFenceEvent = nullptr;
#endif
	};

	// --- Vulkan viewport helpers ---

	static bool VkCreateViewportSwapchain(ViewportData* vd, uint32_t width, uint32_t height)
	{
		VkInstance instance = static_cast<VkInstance>(s_VkHandles.Instance);
		VkPhysicalDevice physical = static_cast<VkPhysicalDevice>(s_VkHandles.PhysicalDevice);
		VkDevice device = static_cast<VkDevice>(s_VkHandles.Device);

		// Query surface capabilities
		VkSurfaceCapabilitiesKHR caps;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, vd->VkSurface, &caps);

		uint32_t fmtCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical, vd->VkSurface, &fmtCount, nullptr);
		std::vector<VkSurfaceFormatKHR> formats(fmtCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical, vd->VkSurface, &fmtCount, formats.data());

		// Prefer UNORM (matches main window)
		VkSurfaceFormatKHR surfaceFormat = formats[0];
		for (const auto& f : formats)
		{
			if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				surfaceFormat = f;
				break;
			}
		}
		vd->VkFormat = surfaceFormat.format;

		VkExtent2D extent;
		if (caps.currentExtent.width != UINT32_MAX)
			extent = caps.currentExtent;
		else
		{
			extent.width = std::max(caps.minImageExtent.width, std::min(caps.maxImageExtent.width, width));
			extent.height = std::max(caps.minImageExtent.height, std::min(caps.maxImageExtent.height, height));
		}
		vd->Width = extent.width;
		vd->Height = extent.height;

		uint32_t imageCount = caps.minImageCount + 1;
		if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
			imageCount = caps.maxImageCount;
		vd->BackbufferCount = imageCount;

		VkSwapchainCreateInfoKHR scInfo{};
		scInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		scInfo.surface = vd->VkSurface;
		scInfo.minImageCount = imageCount;
		scInfo.imageFormat = surfaceFormat.format;
		scInfo.imageColorSpace = surfaceFormat.colorSpace;
		scInfo.imageExtent = extent;
		scInfo.imageArrayLayers = 1;
		scInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		scInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		scInfo.preTransform = caps.currentTransform;
		scInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		scInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		scInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(device, &scInfo, nullptr, &vd->VkSwapchain) != VK_SUCCESS)
		{
			LUMINA_LOG_ERROR("Failed to create viewport Vulkan swapchain");
			return false;
		}

		// Get images
		vkGetSwapchainImagesKHR(device, vd->VkSwapchain, &imageCount, nullptr);
		vd->VkImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, vd->VkSwapchain, &imageCount, vd->VkImages.data());
		vd->BackbufferCount = imageCount;

		// Create fences
		vd->VkFences.resize(imageCount);
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		for (uint32_t i = 0; i < imageCount; ++i)
		{
			if (vkCreateFence(device, &fenceInfo, nullptr, &vd->VkFences[i]) != VK_SUCCESS)
			{
				LUMINA_LOG_ERROR("Failed to create viewport fence {}", i);
				return false;
			}
		}

		// Wrap as NVRHI textures/framebuffers
		nvrhi::Format nvrhiFmt = nvrhi::Format::BGRA8_UNORM;
		if (vd->VkFormat == VK_FORMAT_R8G8B8A8_SRGB || vd->VkFormat == VK_FORMAT_B8G8R8A8_SRGB)
			nvrhiFmt = nvrhi::Format::SBGRA8_UNORM;

		vd->SwapchainTextures.resize(imageCount);
		vd->Framebuffers.resize(imageCount);

		for (uint32_t i = 0; i < imageCount; ++i)
		{
			nvrhi::TextureDesc td;
			td.dimension = nvrhi::TextureDimension::Texture2D;
			td.format = nvrhiFmt;
			td.width = vd->Width;
			td.height = vd->Height;
			td.isRenderTarget = true;
			td.debugName = "Viewport Swapchain " + std::to_string(i);
			td.initialState = nvrhi::ResourceStates::Present;
			td.keepInitialState = true;

			vd->SwapchainTextures[i] = s_Device->createHandleForNativeTexture(
				nvrhi::ObjectTypes::VK_Image, nvrhi::Object(vd->VkImages[i]), td);

			nvrhi::FramebufferDesc fbDesc;
			fbDesc.addColorAttachment(vd->SwapchainTextures[i]);
			vd->Framebuffers[i] = s_Device->createFramebuffer(fbDesc);
		}

		return true;
	}

	static void VkDestroyViewportSwapchain(ViewportData* vd)
	{
		VkDevice device = static_cast<VkDevice>(s_VkHandles.Device);

		vd->Framebuffers.clear();
		vd->SwapchainTextures.clear();

		for (auto& fence : vd->VkFences)
			if (fence) vkDestroyFence(device, fence, nullptr);
		vd->VkFences.clear();

		if (vd->VkSwapchain)
		{
			vkDestroySwapchainKHR(device, vd->VkSwapchain, nullptr);
			vd->VkSwapchain = VK_NULL_HANDLE;
		}

		vd->VkImages.clear();
	}

	// --- D3D12 viewport helpers ---

#ifdef LUMINA_PLATFORM_WINDOWS
	static bool DxCreateViewportSwapchain(ViewportData* vd, HWND hwnd, uint32_t width, uint32_t height)
	{
		auto* factory = static_cast<IDXGIFactory6*>(s_DxHandles.DXGIFactory);
		auto* cmdQueue = static_cast<ID3D12CommandQueue*>(s_DxHandles.CommandQueue);
		auto* d3dDevice = static_cast<ID3D12Device*>(s_DxHandles.Device);

		vd->Width = width;
		vd->Height = height;
		vd->BackbufferCount = 2;

		DXGI_SWAP_CHAIN_DESC1 desc{};
		desc.Width = width;
		desc.Height = height;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = vd->BackbufferCount;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		Microsoft::WRL::ComPtr<IDXGISwapChain1> sc1;
		HRESULT hr = factory->CreateSwapChainForHwnd(cmdQueue, hwnd, &desc, nullptr, nullptr, &sc1);
		if (FAILED(hr))
		{
			LUMINA_LOG_ERROR("Failed to create viewport D3D12 swapchain");
			return false;
		}

		factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		sc1.As(&vd->DxSwapchain);

		// Create fence
		vd->DxFenceValues.resize(vd->BackbufferCount, 0);
		vd->DxCurrentFenceValue = 1;
		d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&vd->DxFence));
		vd->DxFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

		// Wrap backbuffers as NVRHI
		vd->SwapchainTextures.resize(vd->BackbufferCount);
		vd->Framebuffers.resize(vd->BackbufferCount);
		vd->FrameIndex = vd->DxSwapchain->GetCurrentBackBufferIndex();

		for (UINT i = 0; i < vd->BackbufferCount; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backbuffer;
			vd->DxSwapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffer));

			nvrhi::TextureDesc td;
			td.dimension = nvrhi::TextureDimension::Texture2D;
			td.format = nvrhi::Format::RGBA8_UNORM;
			td.width = width;
			td.height = height;
			td.isRenderTarget = true;
			td.debugName = "Viewport DX Swapchain " + std::to_string(i);
			td.initialState = nvrhi::ResourceStates::Present;
			td.keepInitialState = true;

			vd->SwapchainTextures[i] = s_Device->createHandleForNativeTexture(
				nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(backbuffer.Get()), td);

			nvrhi::FramebufferDesc fbDesc;
			fbDesc.addColorAttachment(vd->SwapchainTextures[i]);
			vd->Framebuffers[i] = s_Device->createFramebuffer(fbDesc);
		}

		return true;
	}

	static void DxDestroyViewportSwapchain(ViewportData* vd)
	{
		vd->Framebuffers.clear();
		vd->SwapchainTextures.clear();

		if (vd->DxFenceEvent)
		{
			CloseHandle(vd->DxFenceEvent);
			vd->DxFenceEvent = nullptr;
		}
		vd->DxFence.Reset();
		vd->DxSwapchain.Reset();
		vd->DxFenceValues.clear();
	}
#endif

	// --- ImGui renderer callbacks ---

	static void RendererCreateWindow(ImGuiViewport* vp)
	{
		auto* vd = new ViewportData();
		vp->RendererUserData = vd;

		vd->CommandList = s_Device->createCommandList();

		int w = static_cast<int>(vp->Size.x);
		int h = static_cast<int>(vp->Size.y);

		if (s_GraphicsAPI == nvrhi::GraphicsAPI::VULKAN)
		{
			VkInstance instance = static_cast<VkInstance>(s_VkHandles.Instance);
			auto* glfwWindow = static_cast<GLFWwindow*>(vp->PlatformHandle);

			if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, &vd->VkSurface) != VK_SUCCESS)
			{
				LUMINA_LOG_ERROR("Failed to create Vulkan surface for viewport");
				return;
			}

			VkCreateViewportSwapchain(vd, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
		}
#ifdef LUMINA_PLATFORM_WINDOWS
		else if (s_GraphicsAPI == nvrhi::GraphicsAPI::D3D12)
		{
			HWND hwnd = static_cast<HWND>(vp->PlatformHandleRaw);
			if (!hwnd)
			{
				auto* glfwWindow = static_cast<GLFWwindow*>(vp->PlatformHandle);
				hwnd = glfwGetWin32Window(glfwWindow);
			}

			DxCreateViewportSwapchain(vd, hwnd, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
		}
#endif
	}

	static void RendererDestroyWindow(ImGuiViewport* vp)
	{
		auto* vd = static_cast<ViewportData*>(vp->RendererUserData);
		if (!vd)
			return;

		// Wait for all GPU work to finish
		if (s_GraphicsAPI == nvrhi::GraphicsAPI::VULKAN)
		{
			VkDevice device = static_cast<VkDevice>(s_VkHandles.Device);
			VkQueue queue = static_cast<VkQueue>(s_VkHandles.GraphicsQueue);
			vkQueueWaitIdle(queue);

			VkDestroyViewportSwapchain(vd);

			if (vd->VkSurface)
			{
				VkInstance instance = static_cast<VkInstance>(s_VkHandles.Instance);
				vkDestroySurfaceKHR(instance, vd->VkSurface, nullptr);
				vd->VkSurface = VK_NULL_HANDLE;
			}
		}
#ifdef LUMINA_PLATFORM_WINDOWS
		else if (s_GraphicsAPI == nvrhi::GraphicsAPI::D3D12)
		{
			auto* cmdQueue = static_cast<ID3D12CommandQueue*>(s_DxHandles.CommandQueue);
			// Wait for GPU
			if (vd->DxFence)
			{
				const uint64_t val = vd->DxCurrentFenceValue;
				cmdQueue->Signal(vd->DxFence.Get(), val);
				if (vd->DxFence->GetCompletedValue() < val)
				{
					vd->DxFence->SetEventOnCompletion(val, vd->DxFenceEvent);
					WaitForSingleObjectEx(vd->DxFenceEvent, INFINITE, FALSE);
				}
			}

			DxDestroyViewportSwapchain(vd);
		}
#endif

		vd->CommandList = nullptr;
		delete vd;
		vp->RendererUserData = nullptr;
	}

	static void RendererSetWindowSize(ImGuiViewport* vp, ImVec2 size)
	{
		auto* vd = static_cast<ViewportData*>(vp->RendererUserData);
		if (!vd)
			return;

		uint32_t w = static_cast<uint32_t>(size.x);
		uint32_t h = static_cast<uint32_t>(size.y);

		if (w == 0 || h == 0)
			return;

		if (w == vd->Width && h == vd->Height)
			return;

		if (s_GraphicsAPI == nvrhi::GraphicsAPI::VULKAN)
		{
			VkQueue queue = static_cast<VkQueue>(s_VkHandles.GraphicsQueue);
			vkQueueWaitIdle(queue);

			VkDestroyViewportSwapchain(vd);
			VkCreateViewportSwapchain(vd, w, h);
		}
#ifdef LUMINA_PLATFORM_WINDOWS
		else if (s_GraphicsAPI == nvrhi::GraphicsAPI::D3D12)
		{
			// Wait for GPU
			auto* cmdQueue = static_cast<ID3D12CommandQueue*>(s_DxHandles.CommandQueue);
			if (vd->DxFence)
			{
				const uint64_t val = vd->DxCurrentFenceValue;
				cmdQueue->Signal(vd->DxFence.Get(), val);
				vd->DxCurrentFenceValue++;
				if (vd->DxFence->GetCompletedValue() < val)
				{
					vd->DxFence->SetEventOnCompletion(val, vd->DxFenceEvent);
					WaitForSingleObjectEx(vd->DxFenceEvent, INFINITE, FALSE);
				}
			}

			// Release NVRHI refs before resize
			vd->Framebuffers.clear();
			vd->SwapchainTextures.clear();

			DXGI_SWAP_CHAIN_DESC desc;
			vd->DxSwapchain->GetDesc(&desc);
			vd->DxSwapchain->ResizeBuffers(vd->BackbufferCount, w, h, desc.BufferDesc.Format, desc.Flags);

			vd->Width = w;
			vd->Height = h;
			vd->FrameIndex = vd->DxSwapchain->GetCurrentBackBufferIndex();

			// Re-wrap backbuffers
			vd->SwapchainTextures.resize(vd->BackbufferCount);
			vd->Framebuffers.resize(vd->BackbufferCount);
			for (auto& fv : vd->DxFenceValues) fv = vd->DxFence->GetCompletedValue();

			for (UINT i = 0; i < vd->BackbufferCount; ++i)
			{
				Microsoft::WRL::ComPtr<ID3D12Resource> backbuffer;
				vd->DxSwapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffer));

				nvrhi::TextureDesc td;
				td.dimension = nvrhi::TextureDimension::Texture2D;
				td.format = nvrhi::Format::RGBA8_UNORM;
				td.width = w;
				td.height = h;
				td.isRenderTarget = true;
				td.debugName = "Viewport DX Swapchain " + std::to_string(i);
				td.initialState = nvrhi::ResourceStates::Present;
				td.keepInitialState = true;

				vd->SwapchainTextures[i] = s_Device->createHandleForNativeTexture(
					nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(backbuffer.Get()), td);

				nvrhi::FramebufferDesc fbDesc;
				fbDesc.addColorAttachment(vd->SwapchainTextures[i]);
				vd->Framebuffers[i] = s_Device->createFramebuffer(fbDesc);
			}
		}
#endif
	}

	static void RendererRenderWindow(ImGuiViewport* vp, void*)
	{
		auto* vd = static_cast<ViewportData*>(vp->RendererUserData);
		if (!vd || vd->Width == 0 || vd->Height == 0)
			return;

		if (s_GraphicsAPI == nvrhi::GraphicsAPI::VULKAN)
		{
			VkDevice device = static_cast<VkDevice>(s_VkHandles.Device);
			VkQueue queue = static_cast<VkQueue>(s_VkHandles.GraphicsQueue);

			vkQueueWaitIdle(queue);

			vkResetFences(device, 1, &vd->VkFences[vd->FrameIndex]);

			VkResult result = vkAcquireNextImageKHR(device, vd->VkSwapchain, UINT64_MAX,
				VK_NULL_HANDLE, vd->VkFences[vd->FrameIndex], &vd->ImageIndex);

			if (result == VK_ERROR_OUT_OF_DATE_KHR)
				return;

			vkWaitForFences(device, 1, &vd->VkFences[vd->FrameIndex], VK_TRUE, UINT64_MAX);
		}
#ifdef LUMINA_PLATFORM_WINDOWS
		else if (s_GraphicsAPI == nvrhi::GraphicsAPI::D3D12)
		{
			vd->FrameIndex = vd->DxSwapchain->GetCurrentBackBufferIndex();
			vd->ImageIndex = vd->FrameIndex;

			if (vd->DxFence->GetCompletedValue() < vd->DxFenceValues[vd->FrameIndex])
			{
				vd->DxFence->SetEventOnCompletion(vd->DxFenceValues[vd->FrameIndex], vd->DxFenceEvent);
				WaitForSingleObjectEx(vd->DxFenceEvent, INFINITE, FALSE);
			}
		}
#endif

		// Record and submit rendering commands
		vd->CommandList->open();

		nvrhi::utils::ClearColorAttachment(vd->CommandList, vd->Framebuffers[vd->ImageIndex], 0,
			nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f));

		RenderDrawData(vd->CommandList, vd->Framebuffers[vd->ImageIndex], vp->DrawData);

		vd->CommandList->close();
		s_Device->executeCommandList(vd->CommandList);
	}

	static void RendererSwapBuffers(ImGuiViewport* vp, void*)
	{
		auto* vd = static_cast<ViewportData*>(vp->RendererUserData);
		if (!vd || vd->Width == 0 || vd->Height == 0)
			return;

		if (s_GraphicsAPI == nvrhi::GraphicsAPI::VULKAN)
		{
			VkQueue queue = static_cast<VkQueue>(s_VkHandles.GraphicsQueue);
			vkQueueWaitIdle(queue);

			// Run garbage collection to free staging buffers from viewport rendering
			s_Device->runGarbageCollection();

			VkPresentInfoKHR presentInfo{};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &vd->VkSwapchain;
			presentInfo.pImageIndices = &vd->ImageIndex;

			vkQueuePresentKHR(queue, &presentInfo);
			vd->FrameIndex = (vd->FrameIndex + 1) % vd->BackbufferCount;
		}
#ifdef LUMINA_PLATFORM_WINDOWS
		else if (s_GraphicsAPI == nvrhi::GraphicsAPI::D3D12)
		{
			vd->DxSwapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING);

			auto* cmdQueue = static_cast<ID3D12CommandQueue*>(s_DxHandles.CommandQueue);
			const uint64_t fenceVal = vd->DxCurrentFenceValue;
			cmdQueue->Signal(vd->DxFence.Get(), fenceVal);
			vd->DxFenceValues[vd->FrameIndex] = fenceVal;
			vd->DxCurrentFenceValue++;

			// Run garbage collection to free staging buffers from viewport rendering
			s_Device->runGarbageCollection();

			vd->FrameIndex = vd->DxSwapchain->GetCurrentBackBufferIndex();
		}
#endif
	}

	// --- Public viewport API ---

	void InitPlatformViewports(Device& dev)
	{
		if (s_GraphicsAPI == nvrhi::GraphicsAPI::VULKAN)
			s_VkHandles = dev.GetVulkanHandles();
#ifdef LUMINA_PLATFORM_WINDOWS
		else
			s_DxHandles = dev.GetD3D12Handles();
#endif

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

		ImGuiPlatformIO& pio = ImGui::GetPlatformIO();
		pio.Renderer_CreateWindow = RendererCreateWindow;
		pio.Renderer_DestroyWindow = RendererDestroyWindow;
		pio.Renderer_SetWindowSize = RendererSetWindowSize;
		pio.Renderer_RenderWindow = RendererRenderWindow;
		pio.Renderer_SwapBuffers = RendererSwapBuffers;

		LUMINA_LOG_INFO("ImGui multi-viewport renderer callbacks registered");
	}

	void ShutdownPlatformViewports()
	{
		ImGui::DestroyPlatformWindows();
	}
}
