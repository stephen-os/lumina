#include "imgui_nvrhi.h"
#include "../log.h"
#include "../device.h"

#include <imgui.h>

#include <nvrhi/utils.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#ifdef LUMINA_PLATFORM_WINDOWS
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

namespace lumina::core::imgui
{
    // GLSL shader source for Vulkan (compiled with glslc)
    static const char* s_imgui_vs_glsl = R"(
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

    static const char* s_imgui_ps_glsl = R"(
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
    static const char* s_imgui_vs_hlsl = R"(
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

    static const char* s_imgui_ps_hlsl = R"(
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
    static const char* s_imgui_shader_hlsl_d3d12 = R"(
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

    static nvrhi::GraphicsAPI s_graphics_api = nvrhi::GraphicsAPI::D3D12;

#ifdef LUMINA_PLATFORM_WINDOWS
    // Helper to compile GLSL to SPIR-V using glslc (Vulkan SDK)
    static bool compile_glsl_to_spirv(const char* source, const char* shader_type, std::vector<uint8_t>& spirv_out)
    {
        // Create temporary files for input and output
        std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
        std::filesystem::path input_path = temp_dir / ("imgui_shader_" + std::string(shader_type) + ".glsl");
        std::filesystem::path output_path = temp_dir / ("imgui_shader_" + std::string(shader_type) + ".spv");

        // Write source to temporary file
        {
            std::ofstream input_file(input_path, std::ios::binary);
            if (!input_file)
            {
                LUMINA_LOG_ERROR("Failed to create temporary shader file: {}", input_path.string());
                return false;
            }
            input_file.write(source, strlen(source));
        }

        // Convert paths to use forward slashes for command line compatibility
        std::string input_str = input_path.string();
        std::string output_str = output_path.string();
        std::replace(input_str.begin(), input_str.end(), '\\', '/');
        std::replace(output_str.begin(), output_str.end(), '\\', '/');

        // Build glslc command
        // -fshader-stage specifies the shader type
        // -o specifies output file
        std::string stage = (strcmp(shader_type, "vert") == 0) ? "vertex" : "fragment";
        std::string command = "glslc.exe -fshader-stage=" + stage + " \"" + input_str + "\" -o \"" + output_str + "\" 2>&1";

        LUMINA_LOG_INFO("Compiling {} shader with glslc...", stage);

        // Execute glslc using system() which is more reliable on Windows
        int result = system(command.c_str());

        std::filesystem::remove(input_path);

        if (result != 0)
        {
            LUMINA_LOG_ERROR("glslc compilation failed with exit code: {}", result);
            return false;
        }

        // Read compiled SPIR-V
        std::ifstream spirv_file(output_path, std::ios::binary | std::ios::ate);
        if (!spirv_file)
        {
            LUMINA_LOG_ERROR("Failed to read compiled SPIR-V from: {}", output_path.string());
            return false;
        }

        size_t file_size = spirv_file.tellg();
        spirv_file.seekg(0);

        spirv_out.resize(file_size);
        spirv_file.read(reinterpret_cast<char*>(spirv_out.data()), file_size);
        spirv_file.close();

        std::filesystem::remove(output_path);

        LUMINA_LOG_INFO("Shader compiled successfully ({} bytes)", file_size);
        return true;
    }
#endif

    // Renderer state
    static nvrhi::DeviceHandle s_device;
    static nvrhi::ShaderHandle s_vertex_shader;
    static nvrhi::ShaderHandle s_pixel_shader;
    static nvrhi::InputLayoutHandle s_input_layout;
    static nvrhi::BindingLayoutHandle s_binding_layout;
    static nvrhi::GraphicsPipelineHandle s_pipeline;
    static nvrhi::BufferHandle s_vertex_buffer;
    static nvrhi::BufferHandle s_index_buffer;
    static nvrhi::BufferHandle s_constant_buffer;
    static nvrhi::TextureHandle s_font_texture;
    static nvrhi::SamplerHandle s_font_sampler;
    static nvrhi::BindingSetHandle s_binding_set;

    static uint32_t s_vertex_buffer_size = 0;
    static uint32_t s_index_buffer_size = 0;
    static nvrhi::Format s_render_target_format = nvrhi::Format::RGBA8_UNORM;

    static bool create_font_texture()
    {
        ImGuiIO& io = ImGui::GetIO();

        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        nvrhi::TextureDesc texture_desc;
        texture_desc.dimension = nvrhi::TextureDimension::Texture2D;
        texture_desc.width = static_cast<uint32_t>(width);
        texture_desc.height = static_cast<uint32_t>(height);
        texture_desc.format = nvrhi::Format::RGBA8_UNORM;
        texture_desc.debugName = "ImGui Font Texture";
        texture_desc.initialState = nvrhi::ResourceStates::ShaderResource;
        texture_desc.keepInitialState = true;

        s_font_texture = s_device->createTexture(texture_desc);
        if (!s_font_texture)
        {
            LUMINA_LOG_ERROR("Failed to create ImGui font texture");
            return false;
        }

        // Upload font texture data
        nvrhi::CommandListHandle cmd = s_device->createCommandList();
        cmd->open();
        cmd->writeTexture(s_font_texture, 0, 0, pixels, width * 4);
        cmd->close();
        s_device->executeCommandList(cmd);

        // Store texture ID for ImGui
        io.Fonts->SetTexID(reinterpret_cast<ImTextureID>(s_font_texture.Get()));

        return true;
    }

    static bool create_shaders()
    {
        if (s_graphics_api == nvrhi::GraphicsAPI::VULKAN)
        {
#ifdef LUMINA_PLATFORM_WINDOWS
            // Use glslc to compile GLSL to SPIR-V for Vulkan
            std::vector<uint8_t> vs_spirv;
            if (!compile_glsl_to_spirv(s_imgui_vs_glsl, "vert", vs_spirv))
            {
                LUMINA_LOG_ERROR("Failed to compile ImGui vertex shader to SPIR-V");
                return false;
            }

            nvrhi::ShaderDesc vs_desc;
            vs_desc.shaderType = nvrhi::ShaderType::Vertex;
            vs_desc.debugName = "ImGui VS";

            s_vertex_shader = s_device->createShader(vs_desc, vs_spirv.data(), vs_spirv.size());
            if (!s_vertex_shader)
            {
                LUMINA_LOG_ERROR("Failed to create ImGui vertex shader (SPIR-V)");
                return false;
            }

            std::vector<uint8_t> ps_spirv;
            if (!compile_glsl_to_spirv(s_imgui_ps_glsl, "frag", ps_spirv))
            {
                LUMINA_LOG_ERROR("Failed to compile ImGui pixel shader to SPIR-V");
                return false;
            }

            nvrhi::ShaderDesc ps_desc;
            ps_desc.shaderType = nvrhi::ShaderType::Pixel;
            ps_desc.debugName = "ImGui PS";

            s_pixel_shader = s_device->createShader(ps_desc, ps_spirv.data(), ps_spirv.size());
            if (!s_pixel_shader)
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
        ID3DBlob* vs_blob = nullptr;
        ID3DBlob* error_blob = nullptr;

        HRESULT hr = D3DCompile(
            s_imgui_shader_hlsl_d3d12,
            strlen(s_imgui_shader_hlsl_d3d12),
            "imgui_vs",
            nullptr,
            nullptr,
            "VSMain",
            "vs_5_0",
            D3DCOMPILE_OPTIMIZATION_LEVEL3,
            0,
            &vs_blob,
            &error_blob);

        if (FAILED(hr))
        {
            if (error_blob)
            {
                LUMINA_LOG_ERROR("Failed to compile ImGui vertex shader: {}", (char*)error_blob->GetBufferPointer());
                error_blob->Release();
            }
            return false;
        }

        nvrhi::ShaderDesc vs_desc;
        vs_desc.shaderType = nvrhi::ShaderType::Vertex;
        vs_desc.debugName = "ImGui VS";

        s_vertex_shader = s_device->createShader(vs_desc, vs_blob->GetBufferPointer(), vs_blob->GetBufferSize());
        vs_blob->Release();

        if (!s_vertex_shader)
        {
            LUMINA_LOG_ERROR("Failed to create ImGui vertex shader");
            return false;
        }

        // Compile pixel shader
        ID3DBlob* ps_blob = nullptr;

        hr = D3DCompile(
            s_imgui_shader_hlsl_d3d12,
            strlen(s_imgui_shader_hlsl_d3d12),
            "imgui_ps",
            nullptr,
            nullptr,
            "PSMain",
            "ps_5_0",
            D3DCOMPILE_OPTIMIZATION_LEVEL3,
            0,
            &ps_blob,
            &error_blob);

        if (FAILED(hr))
        {
            if (error_blob)
            {
                LUMINA_LOG_ERROR("Failed to compile ImGui pixel shader: {}", (char*)error_blob->GetBufferPointer());
                error_blob->Release();
            }
            return false;
        }

        nvrhi::ShaderDesc ps_desc;
        ps_desc.shaderType = nvrhi::ShaderType::Pixel;
        ps_desc.debugName = "ImGui PS";

        s_pixel_shader = s_device->createShader(ps_desc, ps_blob->GetBufferPointer(), ps_blob->GetBufferSize());
        ps_blob->Release();

        if (!s_pixel_shader)
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

    static bool create_pipeline()
    {
        // Create input layout
        nvrhi::VertexAttributeDesc vertex_attributes[] = {
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

        s_input_layout = s_device->createInputLayout(vertex_attributes, 3, s_vertex_shader);
        if (!s_input_layout)
        {
            LUMINA_LOG_ERROR("Failed to create ImGui input layout");
            return false;
        }

        // Create binding layout
        nvrhi::BindingLayoutDesc binding_layout_desc;
        binding_layout_desc.visibility = nvrhi::ShaderType::All;

        // For Vulkan, set custom binding offsets so resources map to consecutive bindings
        // Shader expects: binding 0 = constant buffer, binding 1 = texture, binding 2 = sampler
        if (s_graphics_api == nvrhi::GraphicsAPI::VULKAN)
        {
            nvrhi::VulkanBindingOffsets offsets;
            offsets.setConstantBufferOffset(0);   // cb(0) -> binding 0
            offsets.setShaderResourceOffset(1);   // t(0) -> binding 1
            offsets.setSamplerOffset(2);          // s(0) -> binding 2
            binding_layout_desc.setBindingOffsets(offsets);
        }

        binding_layout_desc.addItem(nvrhi::BindingLayoutItem::ConstantBuffer(0));
        binding_layout_desc.addItem(nvrhi::BindingLayoutItem::Texture_SRV(0));
        binding_layout_desc.addItem(nvrhi::BindingLayoutItem::Sampler(0));

        s_binding_layout = s_device->createBindingLayout(binding_layout_desc);
        if (!s_binding_layout)
        {
            LUMINA_LOG_ERROR("Failed to create ImGui binding layout");
            return false;
        }

        // Create constant buffer (non-volatile since we only write once per frame)
        nvrhi::BufferDesc cb_desc;
        cb_desc.byteSize = sizeof(float) * 16;
        cb_desc.isConstantBuffer = true;
        cb_desc.isVolatile = false;
        cb_desc.debugName = "ImGui Constant Buffer";
        cb_desc.initialState = nvrhi::ResourceStates::ConstantBuffer;
        cb_desc.keepInitialState = true;

        s_constant_buffer = s_device->createBuffer(cb_desc);
        if (!s_constant_buffer)
        {
            LUMINA_LOG_ERROR("Failed to create ImGui constant buffer");
            return false;
        }

        // Create sampler
        nvrhi::SamplerDesc sampler_desc;
        sampler_desc.setAllFilters(true);
        sampler_desc.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap);

        s_font_sampler = s_device->createSampler(sampler_desc);
        if (!s_font_sampler)
        {
            LUMINA_LOG_ERROR("Failed to create ImGui sampler");
            return false;
        }

        // Create binding set
        nvrhi::BindingSetDesc binding_set_desc;
        binding_set_desc.addItem(nvrhi::BindingSetItem::ConstantBuffer(0, s_constant_buffer));
        binding_set_desc.addItem(nvrhi::BindingSetItem::Texture_SRV(0, s_font_texture));
        binding_set_desc.addItem(nvrhi::BindingSetItem::Sampler(0, s_font_sampler));

        s_binding_set = s_device->createBindingSet(binding_set_desc, s_binding_layout);
        if (!s_binding_set)
        {
            LUMINA_LOG_ERROR("Failed to create ImGui binding set");
            return false;
        }

        // Create graphics pipeline
        nvrhi::GraphicsPipelineDesc pipeline_desc;
        pipeline_desc.inputLayout = s_input_layout;
        pipeline_desc.VS = s_vertex_shader;
        pipeline_desc.PS = s_pixel_shader;
        pipeline_desc.primType = nvrhi::PrimitiveType::TriangleList;
        pipeline_desc.addBindingLayout(s_binding_layout);

        // Blending
        pipeline_desc.renderState.blendState.targets[0].blendEnable = true;
        pipeline_desc.renderState.blendState.targets[0].srcBlend = nvrhi::BlendFactor::SrcAlpha;
        pipeline_desc.renderState.blendState.targets[0].destBlend = nvrhi::BlendFactor::InvSrcAlpha;
        pipeline_desc.renderState.blendState.targets[0].blendOp = nvrhi::BlendOp::Add;
        pipeline_desc.renderState.blendState.targets[0].srcBlendAlpha = nvrhi::BlendFactor::One;
        pipeline_desc.renderState.blendState.targets[0].destBlendAlpha = nvrhi::BlendFactor::InvSrcAlpha;
        pipeline_desc.renderState.blendState.targets[0].blendOpAlpha = nvrhi::BlendOp::Add;

        // Rasterizer
        pipeline_desc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;
        pipeline_desc.renderState.rasterState.fillMode = nvrhi::RasterFillMode::Solid;
        pipeline_desc.renderState.rasterState.scissorEnable = true;

        // Depth
        pipeline_desc.renderState.depthStencilState.depthTestEnable = false;
        pipeline_desc.renderState.depthStencilState.depthWriteEnable = false;

        nvrhi::FramebufferInfo fb_info;
        fb_info.addColorFormat(s_render_target_format);

        s_pipeline = s_device->createGraphicsPipeline(pipeline_desc, fb_info);
        if (!s_pipeline)
        {
            LUMINA_LOG_ERROR("Failed to create ImGui graphics pipeline");
            return false;
        }

        return true;
    }

    static bool create_buffers(uint32_t vertex_count, uint32_t index_count)
    {
        // Create or resize vertex buffer
        if (!s_vertex_buffer || s_vertex_buffer_size < vertex_count)
        {
            s_vertex_buffer_size = vertex_count + 5000;

            nvrhi::BufferDesc vb_desc;
            vb_desc.byteSize = s_vertex_buffer_size * sizeof(ImDrawVert);
            vb_desc.isVertexBuffer = true;
            vb_desc.debugName = "ImGui Vertex Buffer";
            vb_desc.initialState = nvrhi::ResourceStates::CopyDest;
            vb_desc.keepInitialState = true;

            s_vertex_buffer = s_device->createBuffer(vb_desc);
            if (!s_vertex_buffer)
            {
                LUMINA_LOG_ERROR("Failed to create ImGui vertex buffer");
                return false;
            }
        }

        // Create or resize index buffer
        if (!s_index_buffer || s_index_buffer_size < index_count)
        {
            s_index_buffer_size = index_count + 10000;

            nvrhi::BufferDesc ib_desc;
            ib_desc.byteSize = s_index_buffer_size * sizeof(ImDrawIdx);
            ib_desc.isIndexBuffer = true;
            ib_desc.debugName = "ImGui Index Buffer";
            ib_desc.initialState = nvrhi::ResourceStates::CopyDest;
            ib_desc.keepInitialState = true;

            s_index_buffer = s_device->createBuffer(ib_desc);
            if (!s_index_buffer)
            {
                LUMINA_LOG_ERROR("Failed to create ImGui index buffer");
                return false;
            }
        }

        return true;
    }

    bool init(const imgui_nvrhi_config& config)
    {
        s_device = config.device;
        s_render_target_format = config.render_target_format;
        s_vertex_buffer_size = config.initial_vertex_buffer_size;
        s_index_buffer_size = config.initial_index_buffer_size;
        s_graphics_api = s_device->getGraphicsAPI();

        if (!create_shaders())
        {
            LUMINA_LOG_ERROR("Failed to create ImGui shaders");
            return false;
        }

        if (!create_font_texture())
        {
            LUMINA_LOG_ERROR("Failed to create ImGui font texture");
            return false;
        }

        if (!create_pipeline())
        {
            LUMINA_LOG_ERROR("Failed to create ImGui pipeline");
            return false;
        }

        LUMINA_LOG_INFO("ImGui NVRHI backend initialized");
        return true;
    }

    void shutdown()
    {
        s_binding_set = nullptr;
        s_font_sampler = nullptr;
        s_font_texture = nullptr;
        s_constant_buffer = nullptr;
        s_index_buffer = nullptr;
        s_vertex_buffer = nullptr;
        s_pipeline = nullptr;
        s_binding_layout = nullptr;
        s_input_layout = nullptr;
        s_pixel_shader = nullptr;
        s_vertex_shader = nullptr;
        s_device = nullptr;

        ImGui::GetIO().Fonts->SetTexID(nullptr);

        LUMINA_LOG_INFO("ImGui NVRHI backend shutdown");
    }

    void new_frame()
    {
        // Nothing to do here for NVRHI backend
    }

    void render_draw_data(nvrhi::ICommandList* command_list, nvrhi::IFramebuffer* framebuffer, ImDrawData* draw_data)
    {
        if (!draw_data || draw_data->TotalVtxCount == 0)
            return;

        // Create/resize buffers if needed
        if (!create_buffers(static_cast<uint32_t>(draw_data->TotalVtxCount), static_cast<uint32_t>(draw_data->TotalIdxCount)))
            return;

        // Upload vertex/index data
        ImDrawVert* vtx_dst = nullptr;
        ImDrawIdx* idx_dst = nullptr;

        std::vector<ImDrawVert> vertex_data(draw_data->TotalVtxCount);
        std::vector<ImDrawIdx> index_data(draw_data->TotalIdxCount);

        vtx_dst = vertex_data.data();
        idx_dst = index_data.data();

        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];
            memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtx_dst += cmd_list->VtxBuffer.Size;
            idx_dst += cmd_list->IdxBuffer.Size;
        }

        command_list->writeBuffer(s_vertex_buffer, vertex_data.data(), vertex_data.size() * sizeof(ImDrawVert));
        command_list->writeBuffer(s_index_buffer, index_data.data(), index_data.size() * sizeof(ImDrawIdx));

        // Setup orthographic projection matrix
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

        float mvp[4][4] = {
            { 2.0f / (R - L),     0.0f,              0.0f, 0.0f },
            { 0.0f,               2.0f / (T - B),    0.0f, 0.0f },
            { 0.0f,               0.0f,              0.5f, 0.0f },
            { (R + L) / (L - R),  (T + B) / (B - T), 0.5f, 1.0f },
        };

        command_list->writeBuffer(s_constant_buffer, mvp, sizeof(mvp));

        // Setup viewport
        nvrhi::Viewport viewport;
        viewport.minX = 0;
        viewport.minY = 0;
        viewport.maxX = draw_data->DisplaySize.x;
        viewport.maxY = draw_data->DisplaySize.y;
        viewport.minZ = 0.0f;
        viewport.maxZ = 1.0f;

        // Set graphics state
        nvrhi::GraphicsState state;
        state.pipeline = s_pipeline;
        state.framebuffer = framebuffer;
        state.viewport.addViewport(viewport);
        state.viewport.addScissorRect(nvrhi::Rect(0, static_cast<int>(draw_data->DisplaySize.x), 0, static_cast<int>(draw_data->DisplaySize.y)));
        state.addBindingSet(s_binding_set);
        state.addVertexBuffer({ s_vertex_buffer, 0, 0 });
        state.indexBuffer = { s_index_buffer, nvrhi::Format::R16_UINT, 0 };

        command_list->setGraphicsState(state);

        // Render draw lists
        ImVec2 clip_off = draw_data->DisplayPos;
        int global_vtx_offset = 0;
        int global_idx_offset = 0;

        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

                if (pcmd->UserCallback)
                {
                    pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    // Apply scissor rect
                    ImVec2 clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
                    ImVec2 clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);

                    if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                        continue;

                    nvrhi::Rect scissor;
                    scissor.minX = static_cast<int>(clip_min.x);
                    scissor.minY = static_cast<int>(clip_min.y);
                    scissor.maxX = static_cast<int>(clip_max.x);
                    scissor.maxY = static_cast<int>(clip_max.y);

                    state.viewport.scissorRects[0] = scissor;
                    command_list->setGraphicsState(state);

                    // Draw
                    nvrhi::DrawArguments args;
                    args.vertexCount = pcmd->ElemCount;
                    args.startIndexLocation = pcmd->IdxOffset + global_idx_offset;
                    args.startVertexLocation = pcmd->VtxOffset + global_vtx_offset;

                    command_list->drawIndexed(args);
                }
            }

            global_idx_offset += cmd_list->IdxBuffer.Size;
            global_vtx_offset += cmd_list->VtxBuffer.Size;
        }
    }

    // =========================================================================
    // Multi-viewport support
    // =========================================================================

    // Native handles cached from graphics_device
    static vulkan_native_handles s_vk_handles;
#ifdef LUMINA_PLATFORM_WINDOWS
    static d3d12_native_handles s_dx_handles;
#endif

    // Per-viewport renderer data stored in ImGuiViewport::RendererUserData
    struct viewport_data
    {
        nvrhi::CommandListHandle command_list;
        std::vector<nvrhi::TextureHandle> swapchain_textures;
        std::vector<nvrhi::FramebufferHandle> framebuffers;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t backbuffer_count = 2;
        uint32_t frame_index = 0;
        uint32_t image_index = 0;

        // Vulkan-specific
        VkSurfaceKHR vk_surface = VK_NULL_HANDLE;
        VkSwapchainKHR vk_swapchain = VK_NULL_HANDLE;
        VkFormat vk_format = VK_FORMAT_UNDEFINED;
        std::vector<VkImage> vk_images;
        std::vector<VkFence> vk_fences;

        // D3D12-specific
#ifdef LUMINA_PLATFORM_WINDOWS
        Microsoft::WRL::ComPtr<IDXGISwapChain4> dx_swapchain;
        Microsoft::WRL::ComPtr<ID3D12Fence> dx_fence;
        std::vector<uint64_t> dx_fence_values;
        uint64_t dx_current_fence_value = 1;
        HANDLE dx_fence_event = nullptr;
#endif
    };

    // --- Vulkan viewport helpers ---

    static bool vk_create_viewport_swapchain(viewport_data* vd, uint32_t width, uint32_t height)
    {
        VkInstance instance = static_cast<VkInstance>(s_vk_handles.instance);
        VkPhysicalDevice physical = static_cast<VkPhysicalDevice>(s_vk_handles.physical_device);
        VkDevice device = static_cast<VkDevice>(s_vk_handles.device);

        // Query surface capabilities
        VkSurfaceCapabilitiesKHR caps;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical, vd->vk_surface, &caps);

        uint32_t fmt_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical, vd->vk_surface, &fmt_count, nullptr);
        std::vector<VkSurfaceFormatKHR> formats(fmt_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical, vd->vk_surface, &fmt_count, formats.data());

        // Prefer UNORM (matches main window)
        VkSurfaceFormatKHR surface_format = formats[0];
        for (const auto& f : formats)
        {
            if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                surface_format = f;
                break;
            }
        }
        vd->vk_format = surface_format.format;

        VkExtent2D extent;
        if (caps.currentExtent.width != UINT32_MAX)
            extent = caps.currentExtent;
        else
        {
            extent.width = std::max(caps.minImageExtent.width, std::min(caps.maxImageExtent.width, width));
            extent.height = std::max(caps.minImageExtent.height, std::min(caps.maxImageExtent.height, height));
        }
        vd->width = extent.width;
        vd->height = extent.height;

        uint32_t image_count = caps.minImageCount + 1;
        if (caps.maxImageCount > 0 && image_count > caps.maxImageCount)
            image_count = caps.maxImageCount;
        vd->backbuffer_count = image_count;

        VkSwapchainCreateInfoKHR sc_info{};
        sc_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        sc_info.surface = vd->vk_surface;
        sc_info.minImageCount = image_count;
        sc_info.imageFormat = surface_format.format;
        sc_info.imageColorSpace = surface_format.colorSpace;
        sc_info.imageExtent = extent;
        sc_info.imageArrayLayers = 1;
        sc_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        sc_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        sc_info.preTransform = caps.currentTransform;
        sc_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        sc_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        sc_info.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(device, &sc_info, nullptr, &vd->vk_swapchain) != VK_SUCCESS)
        {
            LUMINA_LOG_ERROR("Failed to create viewport Vulkan swapchain");
            return false;
        }

        // Get images
        vkGetSwapchainImagesKHR(device, vd->vk_swapchain, &image_count, nullptr);
        vd->vk_images.resize(image_count);
        vkGetSwapchainImagesKHR(device, vd->vk_swapchain, &image_count, vd->vk_images.data());
        vd->backbuffer_count = image_count;

        // Create fences
        vd->vk_fences.resize(image_count);
        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (uint32_t i = 0; i < image_count; ++i)
            vkCreateFence(device, &fence_info, nullptr, &vd->vk_fences[i]);

        // Wrap as NVRHI textures/framebuffers
        nvrhi::Format nvrhi_fmt = nvrhi::Format::BGRA8_UNORM;
        if (vd->vk_format == VK_FORMAT_R8G8B8A8_SRGB || vd->vk_format == VK_FORMAT_B8G8R8A8_SRGB)
            nvrhi_fmt = nvrhi::Format::SBGRA8_UNORM;

        vd->swapchain_textures.resize(image_count);
        vd->framebuffers.resize(image_count);

        for (uint32_t i = 0; i < image_count; ++i)
        {
            nvrhi::TextureDesc td;
            td.dimension = nvrhi::TextureDimension::Texture2D;
            td.format = nvrhi_fmt;
            td.width = vd->width;
            td.height = vd->height;
            td.isRenderTarget = true;
            td.debugName = "Viewport Swapchain " + std::to_string(i);
            td.initialState = nvrhi::ResourceStates::Present;
            td.keepInitialState = true;

            vd->swapchain_textures[i] = s_device->createHandleForNativeTexture(
                nvrhi::ObjectTypes::VK_Image, nvrhi::Object(vd->vk_images[i]), td);

            nvrhi::FramebufferDesc fb_desc;
            fb_desc.addColorAttachment(vd->swapchain_textures[i]);
            vd->framebuffers[i] = s_device->createFramebuffer(fb_desc);
        }

        return true;
    }

    static void vk_destroy_viewport_swapchain(viewport_data* vd)
    {
        VkDevice device = static_cast<VkDevice>(s_vk_handles.device);

        vd->framebuffers.clear();
        vd->swapchain_textures.clear();

        for (auto& fence : vd->vk_fences)
            if (fence) vkDestroyFence(device, fence, nullptr);
        vd->vk_fences.clear();

        if (vd->vk_swapchain)
        {
            vkDestroySwapchainKHR(device, vd->vk_swapchain, nullptr);
            vd->vk_swapchain = VK_NULL_HANDLE;
        }

        vd->vk_images.clear();
    }

    // --- D3D12 viewport helpers ---

#ifdef LUMINA_PLATFORM_WINDOWS
    static bool dx_create_viewport_swapchain(viewport_data* vd, HWND hwnd, uint32_t width, uint32_t height)
    {
        auto* factory = static_cast<IDXGIFactory6*>(s_dx_handles.dxgi_factory);
        auto* cmd_queue = static_cast<ID3D12CommandQueue*>(s_dx_handles.command_queue);
        auto* d3d_device = static_cast<ID3D12Device*>(s_dx_handles.device);

        vd->width = width;
        vd->height = height;
        vd->backbuffer_count = 2;

        DXGI_SWAP_CHAIN_DESC1 desc{};
        desc.Width = width;
        desc.Height = height;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.BufferCount = vd->backbuffer_count;
        desc.Scaling = DXGI_SCALING_STRETCH;
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

        Microsoft::WRL::ComPtr<IDXGISwapChain1> sc1;
        HRESULT hr = factory->CreateSwapChainForHwnd(cmd_queue, hwnd, &desc, nullptr, nullptr, &sc1);
        if (FAILED(hr))
        {
            LUMINA_LOG_ERROR("Failed to create viewport D3D12 swapchain");
            return false;
        }

        factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
        sc1.As(&vd->dx_swapchain);

        // Create fence
        vd->dx_fence_values.resize(vd->backbuffer_count, 0);
        vd->dx_current_fence_value = 1;
        d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&vd->dx_fence));
        vd->dx_fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);

        // Wrap backbuffers as NVRHI
        vd->swapchain_textures.resize(vd->backbuffer_count);
        vd->framebuffers.resize(vd->backbuffer_count);
        vd->frame_index = vd->dx_swapchain->GetCurrentBackBufferIndex();

        for (UINT i = 0; i < vd->backbuffer_count; ++i)
        {
            Microsoft::WRL::ComPtr<ID3D12Resource> backbuffer;
            vd->dx_swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffer));

            nvrhi::TextureDesc td;
            td.dimension = nvrhi::TextureDimension::Texture2D;
            td.format = nvrhi::Format::RGBA8_UNORM;
            td.width = width;
            td.height = height;
            td.isRenderTarget = true;
            td.debugName = "Viewport DX Swapchain " + std::to_string(i);
            td.initialState = nvrhi::ResourceStates::Present;
            td.keepInitialState = true;

            vd->swapchain_textures[i] = s_device->createHandleForNativeTexture(
                nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(backbuffer.Get()), td);

            nvrhi::FramebufferDesc fb_desc;
            fb_desc.addColorAttachment(vd->swapchain_textures[i]);
            vd->framebuffers[i] = s_device->createFramebuffer(fb_desc);
        }

        return true;
    }

    static void dx_destroy_viewport_swapchain(viewport_data* vd)
    {
        vd->framebuffers.clear();
        vd->swapchain_textures.clear();

        if (vd->dx_fence_event)
        {
            CloseHandle(vd->dx_fence_event);
            vd->dx_fence_event = nullptr;
        }
        vd->dx_fence.Reset();
        vd->dx_swapchain.Reset();
        vd->dx_fence_values.clear();
    }
#endif

    // --- ImGui renderer callbacks ---

    static void renderer_create_window(ImGuiViewport* vp)
    {
        auto* vd = new viewport_data();
        vp->RendererUserData = vd;

        vd->command_list = s_device->createCommandList();

        int w = static_cast<int>(vp->Size.x);
        int h = static_cast<int>(vp->Size.y);

        if (s_graphics_api == nvrhi::GraphicsAPI::VULKAN)
        {
            VkInstance instance = static_cast<VkInstance>(s_vk_handles.instance);
            auto* glfw_window = static_cast<GLFWwindow*>(vp->PlatformHandle);

            if (glfwCreateWindowSurface(instance, glfw_window, nullptr, &vd->vk_surface) != VK_SUCCESS)
            {
                LUMINA_LOG_ERROR("Failed to create Vulkan surface for viewport");
                return;
            }

            vk_create_viewport_swapchain(vd, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
        }
#ifdef LUMINA_PLATFORM_WINDOWS
        else if (s_graphics_api == nvrhi::GraphicsAPI::D3D12)
        {
            HWND hwnd = static_cast<HWND>(vp->PlatformHandleRaw);
            if (!hwnd)
            {
                auto* glfw_window = static_cast<GLFWwindow*>(vp->PlatformHandle);
                hwnd = glfwGetWin32Window(glfw_window);
            }

            dx_create_viewport_swapchain(vd, hwnd, static_cast<uint32_t>(w), static_cast<uint32_t>(h));
        }
#endif
    }

    static void renderer_destroy_window(ImGuiViewport* vp)
    {
        auto* vd = static_cast<viewport_data*>(vp->RendererUserData);
        if (!vd)
            return;

        // Wait for all GPU work to finish
        if (s_graphics_api == nvrhi::GraphicsAPI::VULKAN)
        {
            VkDevice device = static_cast<VkDevice>(s_vk_handles.device);
            VkQueue queue = static_cast<VkQueue>(s_vk_handles.graphics_queue);
            vkQueueWaitIdle(queue);

            vk_destroy_viewport_swapchain(vd);

            if (vd->vk_surface)
            {
                VkInstance instance = static_cast<VkInstance>(s_vk_handles.instance);
                vkDestroySurfaceKHR(instance, vd->vk_surface, nullptr);
                vd->vk_surface = VK_NULL_HANDLE;
            }
        }
#ifdef LUMINA_PLATFORM_WINDOWS
        else if (s_graphics_api == nvrhi::GraphicsAPI::D3D12)
        {
            auto* cmd_queue = static_cast<ID3D12CommandQueue*>(s_dx_handles.command_queue);
            // Wait for GPU
            if (vd->dx_fence)
            {
                const uint64_t val = vd->dx_current_fence_value;
                cmd_queue->Signal(vd->dx_fence.Get(), val);
                if (vd->dx_fence->GetCompletedValue() < val)
                {
                    vd->dx_fence->SetEventOnCompletion(val, vd->dx_fence_event);
                    WaitForSingleObjectEx(vd->dx_fence_event, INFINITE, FALSE);
                }
            }

            dx_destroy_viewport_swapchain(vd);
        }
#endif

        vd->command_list = nullptr;
        delete vd;
        vp->RendererUserData = nullptr;
    }

    static void renderer_set_window_size(ImGuiViewport* vp, ImVec2 size)
    {
        auto* vd = static_cast<viewport_data*>(vp->RendererUserData);
        if (!vd)
            return;

        uint32_t w = static_cast<uint32_t>(size.x);
        uint32_t h = static_cast<uint32_t>(size.y);

        if (w == 0 || h == 0)
            return;

        if (w == vd->width && h == vd->height)
            return;

        if (s_graphics_api == nvrhi::GraphicsAPI::VULKAN)
        {
            VkQueue queue = static_cast<VkQueue>(s_vk_handles.graphics_queue);
            vkQueueWaitIdle(queue);

            vk_destroy_viewport_swapchain(vd);
            vk_create_viewport_swapchain(vd, w, h);
        }
#ifdef LUMINA_PLATFORM_WINDOWS
        else if (s_graphics_api == nvrhi::GraphicsAPI::D3D12)
        {
            // Wait for GPU
            auto* cmd_queue = static_cast<ID3D12CommandQueue*>(s_dx_handles.command_queue);
            if (vd->dx_fence)
            {
                const uint64_t val = vd->dx_current_fence_value;
                cmd_queue->Signal(vd->dx_fence.Get(), val);
                vd->dx_current_fence_value++;
                if (vd->dx_fence->GetCompletedValue() < val)
                {
                    vd->dx_fence->SetEventOnCompletion(val, vd->dx_fence_event);
                    WaitForSingleObjectEx(vd->dx_fence_event, INFINITE, FALSE);
                }
            }

            // Release NVRHI refs before resize
            vd->framebuffers.clear();
            vd->swapchain_textures.clear();

            DXGI_SWAP_CHAIN_DESC desc;
            vd->dx_swapchain->GetDesc(&desc);
            vd->dx_swapchain->ResizeBuffers(vd->backbuffer_count, w, h, desc.BufferDesc.Format, desc.Flags);

            vd->width = w;
            vd->height = h;
            vd->frame_index = vd->dx_swapchain->GetCurrentBackBufferIndex();

            // Re-wrap backbuffers
            vd->swapchain_textures.resize(vd->backbuffer_count);
            vd->framebuffers.resize(vd->backbuffer_count);
            for (auto& fv : vd->dx_fence_values) fv = vd->dx_fence->GetCompletedValue();

            for (UINT i = 0; i < vd->backbuffer_count; ++i)
            {
                Microsoft::WRL::ComPtr<ID3D12Resource> backbuffer;
                vd->dx_swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffer));

                nvrhi::TextureDesc td;
                td.dimension = nvrhi::TextureDimension::Texture2D;
                td.format = nvrhi::Format::RGBA8_UNORM;
                td.width = w;
                td.height = h;
                td.isRenderTarget = true;
                td.debugName = "Viewport DX Swapchain " + std::to_string(i);
                td.initialState = nvrhi::ResourceStates::Present;
                td.keepInitialState = true;

                vd->swapchain_textures[i] = s_device->createHandleForNativeTexture(
                    nvrhi::ObjectTypes::D3D12_Resource, nvrhi::Object(backbuffer.Get()), td);

                nvrhi::FramebufferDesc fb_desc;
                fb_desc.addColorAttachment(vd->swapchain_textures[i]);
                vd->framebuffers[i] = s_device->createFramebuffer(fb_desc);
            }
        }
#endif
    }

    static void renderer_render_window(ImGuiViewport* vp, void*)
    {
        auto* vd = static_cast<viewport_data*>(vp->RendererUserData);
        if (!vd || vd->width == 0 || vd->height == 0)
            return;

        if (s_graphics_api == nvrhi::GraphicsAPI::VULKAN)
        {
            VkDevice device = static_cast<VkDevice>(s_vk_handles.device);
            VkQueue queue = static_cast<VkQueue>(s_vk_handles.graphics_queue);

            vkQueueWaitIdle(queue);

            vkResetFences(device, 1, &vd->vk_fences[vd->frame_index]);

            VkResult result = vkAcquireNextImageKHR(device, vd->vk_swapchain, UINT64_MAX,
                VK_NULL_HANDLE, vd->vk_fences[vd->frame_index], &vd->image_index);

            if (result == VK_ERROR_OUT_OF_DATE_KHR)
                return;

            vkWaitForFences(device, 1, &vd->vk_fences[vd->frame_index], VK_TRUE, UINT64_MAX);
        }
#ifdef LUMINA_PLATFORM_WINDOWS
        else if (s_graphics_api == nvrhi::GraphicsAPI::D3D12)
        {
            vd->frame_index = vd->dx_swapchain->GetCurrentBackBufferIndex();
            vd->image_index = vd->frame_index;

            if (vd->dx_fence->GetCompletedValue() < vd->dx_fence_values[vd->frame_index])
            {
                vd->dx_fence->SetEventOnCompletion(vd->dx_fence_values[vd->frame_index], vd->dx_fence_event);
                WaitForSingleObjectEx(vd->dx_fence_event, INFINITE, FALSE);
            }
        }
#endif

        // Record and submit rendering commands
        vd->command_list->open();

        nvrhi::utils::ClearColorAttachment(vd->command_list, vd->framebuffers[vd->image_index], 0,
            nvrhi::Color(0.0f, 0.0f, 0.0f, 1.0f));

        render_draw_data(vd->command_list, vd->framebuffers[vd->image_index], vp->DrawData);

        vd->command_list->close();
        s_device->executeCommandList(vd->command_list);
    }

    static void renderer_swap_buffers(ImGuiViewport* vp, void*)
    {
        auto* vd = static_cast<viewport_data*>(vp->RendererUserData);
        if (!vd || vd->width == 0 || vd->height == 0)
            return;

        if (s_graphics_api == nvrhi::GraphicsAPI::VULKAN)
        {
            VkQueue queue = static_cast<VkQueue>(s_vk_handles.graphics_queue);
            vkQueueWaitIdle(queue);

            VkPresentInfoKHR present_info{};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            present_info.swapchainCount = 1;
            present_info.pSwapchains = &vd->vk_swapchain;
            present_info.pImageIndices = &vd->image_index;

            vkQueuePresentKHR(queue, &present_info);
            vd->frame_index = (vd->frame_index + 1) % vd->backbuffer_count;
        }
#ifdef LUMINA_PLATFORM_WINDOWS
        else if (s_graphics_api == nvrhi::GraphicsAPI::D3D12)
        {
            vd->dx_swapchain->Present(0, DXGI_PRESENT_ALLOW_TEARING);

            auto* cmd_queue = static_cast<ID3D12CommandQueue*>(s_dx_handles.command_queue);
            const uint64_t fence_val = vd->dx_current_fence_value;
            cmd_queue->Signal(vd->dx_fence.Get(), fence_val);
            vd->dx_fence_values[vd->frame_index] = fence_val;
            vd->dx_current_fence_value++;

            vd->frame_index = vd->dx_swapchain->GetCurrentBackBufferIndex();
        }
#endif
    }

    // --- Public viewport API ---

    void init_platform_viewports(device& dev)
    {
        if (s_graphics_api == nvrhi::GraphicsAPI::VULKAN)
            s_vk_handles = dev.get_vulkan_handles();
#ifdef LUMINA_PLATFORM_WINDOWS
        else
            s_dx_handles = dev.get_d3d12_handles();
#endif

        ImGuiIO& io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

        ImGuiPlatformIO& pio = ImGui::GetPlatformIO();
        pio.Renderer_CreateWindow = renderer_create_window;
        pio.Renderer_DestroyWindow = renderer_destroy_window;
        pio.Renderer_SetWindowSize = renderer_set_window_size;
        pio.Renderer_RenderWindow = renderer_render_window;
        pio.Renderer_SwapBuffers = renderer_swap_buffers;

        LUMINA_LOG_INFO("ImGui multi-viewport renderer callbacks registered");
    }

    void shutdown_platform_viewports()
    {
        ImGui::DestroyPlatformWindows();
    }
}
