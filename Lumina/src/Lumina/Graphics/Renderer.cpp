#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>

#include <stb_image_write.h>

#include "../Core/Log.h"
#include "../Core/Assert.h"

#include "../Utils/FileReader.h"

#include "VertexArray.h"
#include "Buffer.h"
#include "BufferLayout.h"
#include "FrameBuffer.h"
#include "RenderCommands.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <array>

namespace Lumina
{
    // Constants
    constexpr uint32_t MaxQuads = 10000;  // Increased from 1000
	constexpr uint32_t MaxCubes = 10000;  // Increased from 1000
    constexpr uint32_t MaxVertices = MaxQuads * 4;
    constexpr uint32_t MaxIndices = MaxQuads * 6;
    constexpr uint32_t MaxTextureSlots = 32; // Depends on GPU

    // Vertex structure
    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
    };

	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
	};

    // Renderer data storage
    struct RendererData
    {
        // Core renderer resources
        Ref<FrameBuffer> RendererFrameBuffer;

        // Quad
        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<IndexBuffer> QuadIndexBuffer;

        // Cube
		Ref<VertexArray> CubeVertexArray;
		Ref<VertexBuffer> CubeVertexBuffer;
		Ref<IndexBuffer> CubeIndexBuffer;

        // Batch rendering data
        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        uint32_t CubeIndexCount = 0;
        CubeVertex* CubeVertexBufferBase = nullptr;
        CubeVertex* CubeVertexBufferPtr = nullptr;

        // Shaders
		Ref<ShaderProgram> QuadShader = nullptr;
		Ref<ShaderProgram> CubeShader = nullptr;

        // Texture management
        std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 is white texture

        // Quad rendering data
        glm::vec4 QuadVertexPositions[4];
        glm::vec2 TexCoords[4];

        // Cube Renderering
        glm::vec3 CubeVertexPositions[24]; 

        // View-projection matrix for camera support
        glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);

        // Resolution tracking
        uint32_t Width = 800;
        uint32_t Height = 600;

        // Performance statistics
        Renderer::Statistics Stats;
    };

    static RendererData s_Data;

    void Renderer::Init()
    {
        // Create framebuffer
        s_Data.RendererFrameBuffer = FrameBuffer::Create();

        // Create vertex arrays and buffers
        s_Data.QuadVertexArray = VertexArray::Create();
        s_Data.QuadVertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(QuadVertex));

        // Set up buffer layout
        s_Data.QuadVertexBuffer->SetLayout({
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float4, "a_Color" },
            { BufferDataType::Float2, "a_TexCoord" },
            { BufferDataType::Float,  "a_TexIndex" }
        });

        s_Data.QuadVertexArray->SetVertexBuffer(s_Data.QuadVertexBuffer);

        // Generate index buffer for quads
        std::vector<uint32_t> quadIndices(MaxIndices);
        uint32_t offset = 0;
        for (uint32_t i = 0; i < MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        s_Data.QuadIndexBuffer = IndexBuffer::Create(quadIndices.data(), MaxIndices);
        s_Data.QuadVertexArray->SetIndexBuffer(s_Data.QuadIndexBuffer);

        // Allocate vertex buffer memory
        s_Data.QuadVertexBufferBase = new QuadVertex[MaxVertices];

        // Create a default shader
        {
			std::string vertexSource = ReadFile("res/shaders/Quad.vert");
			std::string fragmentSource = ReadFile("res/shaders/Quad.frag");
            s_Data.QuadShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }
        
        // Create a 1x1 white texture for basic colored quads
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.TextureSlots[0] = Texture::Create(1, 1);
        s_Data.TextureSlots[0]->SetData(&whiteTextureData, sizeof(uint32_t));

        // Set up default quad vertex positions in object space
        s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.5f };  // Bottom right
        s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.5f };  // Bottom left
        s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.5f };  // Top right
        s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.5f };  // Top left

        // Set up default texture coordinates
        s_Data.TexCoords[0] = { 0.0f, 0.0f };  // Bottom left
        s_Data.TexCoords[1] = { 1.0f, 0.0f };  // Bottom right
        s_Data.TexCoords[2] = { 1.0f, 1.0f };  // Top right
        s_Data.TexCoords[3] = { 0.0f, 1.0f };  // Top left

        // Cube
		s_Data.CubeVertexArray = VertexArray::Create();
		s_Data.CubeVertexBuffer = VertexBuffer::Create(MaxCubes * sizeof(CubeVertex));

		s_Data.CubeVertexBuffer->SetLayout({
            { BufferDataType::Float3, "a_Position" },
			{ BufferDataType::Float4, "a_Color" },
			{ BufferDataType::Float2, "a_TexCoord" },
			{ BufferDataType::Float,  "a_TexIndex" }
	    });
	    s_Data.CubeVertexArray->SetVertexBuffer(s_Data.CubeVertexBuffer);

        // 36 vertices per cube (6 faces x 2 triangles x 3 vertices)
        s_Data.CubeVertexBufferBase = new CubeVertex[MaxCubes * 36]; 

        // Cube Shader
        {
            std::string vertexSource = ReadFile("res/shaders/Cube.vert");
            std::string fragmentSource = ReadFile("res/shaders/Cube.frag");
            s_Data.CubeShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        std::vector<uint32_t> cubeIndices;
        cubeIndices.reserve(MaxCubes * 36);

        for (uint32_t i = 0; i < MaxCubes; ++i)
        {
            uint32_t offset = i * 24; // 24 vertices if using non-indexed cube (Ref vertices = 8)

            std::array<uint32_t, 36> faceIndices = {
                0, 1, 2, 2, 3, 0, // Front
                4, 5, 6, 6, 7, 4, // Back
                8, 9,10,10,11, 8, // Top
               12,13,14,14,15,12, // Bottom
               16,17,18,18,19,16, // Right
               20,21,22,22,23,20  // Left
            };

            for (uint32_t index : faceIndices)
                cubeIndices.push_back(offset + index);
        }
        s_Data.CubeIndexBuffer = IndexBuffer::Create(cubeIndices.data(), static_cast<uint32_t>(cubeIndices.size()));
        s_Data.CubeVertexArray->SetIndexBuffer(s_Data.CubeIndexBuffer);

        s_Data.CubeVertexPositions[0] = { -0.5f, -0.5f,  0.5f };
        s_Data.CubeVertexPositions[1] = { 0.5f, -0.5f,  0.5f };
        s_Data.CubeVertexPositions[2] = { 0.5f,  0.5f,  0.5f };
        s_Data.CubeVertexPositions[3] = { -0.5f,  0.5f,  0.5f };

        s_Data.CubeVertexPositions[4] = { 0.5f, -0.5f, -0.5f };
        s_Data.CubeVertexPositions[5] = { -0.5f, -0.5f, -0.5f };
        s_Data.CubeVertexPositions[6] = { -0.5f,  0.5f, -0.5f };
        s_Data.CubeVertexPositions[7] = { 0.5f,  0.5f, -0.5f };

        s_Data.CubeVertexPositions[8] = { -0.5f,  0.5f,  0.5f };
        s_Data.CubeVertexPositions[9] = { 0.5f,  0.5f,  0.5f };
        s_Data.CubeVertexPositions[10] = { 0.5f,  0.5f, -0.5f };
        s_Data.CubeVertexPositions[11] = { -0.5f,  0.5f, -0.5f };

        s_Data.CubeVertexPositions[12] = { -0.5f, -0.5f, -0.5f };
        s_Data.CubeVertexPositions[13] = { 0.5f, -0.5f, -0.5f };
        s_Data.CubeVertexPositions[14] = { 0.5f, -0.5f,  0.5f };
        s_Data.CubeVertexPositions[15] = { -0.5f, -0.5f,  0.5f };

        s_Data.CubeVertexPositions[16] = { 0.5f, -0.5f,  0.5f };
        s_Data.CubeVertexPositions[17] = { 0.5f, -0.5f, -0.5f };
        s_Data.CubeVertexPositions[18] = { 0.5f,  0.5f, -0.5f };
        s_Data.CubeVertexPositions[19] = { 0.5f,  0.5f,  0.5f };

        s_Data.CubeVertexPositions[20] = { -0.5f, -0.5f, -0.5f };
        s_Data.CubeVertexPositions[21] = { -0.5f, -0.5f,  0.5f };
        s_Data.CubeVertexPositions[22] = { -0.5f,  0.5f,  0.5f };
        s_Data.CubeVertexPositions[23] = { -0.5f,  0.5f, -0.5f };
    }

    void Renderer::Shutdown()
    {
        // Free allocated memory
        delete[] s_Data.QuadVertexBufferBase;
    }

    void Renderer::Begin(Camera& camera)
    {
        s_Data.ViewProjectionMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
        StartBatch(); 
    }

    void Renderer::Begin(glm::mat4& viewProjection)
    {
        s_Data.ViewProjectionMatrix = viewProjection;
        StartBatch();
    }

    void Renderer::End()
    {
		EndBatch();
    }

	void Renderer::StartBatch()
	{
		s_Data.QuadIndexCount = 0;
		s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

		s_Data.CubeIndexCount = 0;
		s_Data.CubeVertexBufferPtr = s_Data.CubeVertexBufferBase;

		s_Data.TextureSlotIndex = 1;
	}

	void Renderer::EndBatch()
	{
        // Calculate data size and upload to GPU
        uint32_t quadDataSize = static_cast<uint32_t>((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
		uint32_t cubeDataSize = static_cast<uint32_t>((uint8_t*)s_Data.CubeVertexBufferPtr - (uint8_t*)s_Data.CubeVertexBufferBase);
        bool issueDraw = false; 

        if (quadDataSize > 0)
        {
            s_Data.Stats.DataSize += quadDataSize;
            s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, quadDataSize);
			issueDraw = true;
        }

        if (cubeDataSize > 0)
        {
            s_Data.Stats.DataSize += cubeDataSize;
            s_Data.CubeVertexBuffer->SetData(s_Data.CubeVertexBufferBase, cubeDataSize);
			issueDraw = true;
        }

		if (issueDraw)
		{
            Flush();
		}
	}

    void Renderer::Flush()
    {
        // Bind FB
        s_Data.RendererFrameBuffer->Bind();

        // Clear Buffer
        RenderCommands::Clear();

        // Set viewport
        RenderCommands::SetViewport(0, 0, s_Data.Width, s_Data.Height);
        s_Data.RendererFrameBuffer->Resize(s_Data.Width, s_Data.Height); 

        RenderCommands::EnableDepthTest(); 

        // Bind all used textures
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
            s_Data.TextureSlots[i]->Bind(i);

        if (s_Data.QuadIndexCount != 0)
        {
		    s_Data.QuadShader->Bind();
		    s_Data.QuadShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);

            // Draw the Quads
		    s_Data.QuadVertexArray->Bind();
            RenderCommands::DrawTriangles(s_Data.QuadVertexArray); 
		    s_Data.QuadVertexArray->Unbind();

            s_Data.QuadShader->Unbind();
        }

        if (s_Data.CubeIndexCount != 0)
        {
            s_Data.CubeShader->Bind();
            s_Data.CubeShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);

            // Draw Cubes
            s_Data.CubeVertexArray->Bind();
            RenderCommands::DrawTriangles(s_Data.CubeVertexArray);
            s_Data.CubeVertexArray->Unbind();

            s_Data.CubeShader->Unbind();
        }

        // Unbind FB
        s_Data.RendererFrameBuffer->Unbind();

        // Unbind all textures
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
            s_Data.TextureSlots[i]->Unbind();
        
        // Update stats
        s_Data.Stats.DrawCalls++;
        s_Data.Stats.TexturesUsed = s_Data.TextureSlotIndex - 1;
    }

    void Renderer::SetResolution(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0) 
        {
            std::cerr << "Invalid resolution: " << width << "x" << height << std::endl;
            return;
        }

        s_Data.Width = width;
        s_Data.Height = height;
    }

    uint32_t Renderer::GetImage()
    {
        return s_Data.RendererFrameBuffer->GetColorAttachment();
    }

	float Renderer::ComputeTextureIndex(const Ref<Texture>& texture)
	{
		if (texture == nullptr)
			return 0.0f;

		for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
		{
			if (s_Data.TextureSlots[i] == texture)
				return static_cast<float>(i);
		}

        if (s_Data.TextureSlotIndex >= MaxTextureSlots)
        {
            EndBatch();
            StartBatch();
        }

        LUMINA_ASSERT(s_Data.TextureSlotIndex < MaxTextureSlots, "Texture slot index overflow!");

        float texIndex = static_cast<float>(s_Data.TextureSlotIndex);
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = texture;
        s_Data.TextureSlotIndex++;

		return texIndex;
	}

    glm::vec2 Renderer::GetResolution()
    {
        return { s_Data.Width, s_Data.Height };
    }

    void Renderer::DrawQuad(const QuadAttributes& attributes)
    {
        LUMINA_ASSERT(s_Data.QuadVertexBufferPtr >= s_Data.QuadVertexBufferBase, "Vertex buffer pointer underflow");

        if (s_Data.QuadIndexCount >= MaxIndices)
        {
            EndBatch();
            StartBatch();
        }

        // Compute translation 
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), attributes.Position);
        
		// Compute rotation
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), attributes.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), attributes.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), attributes.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 rotation = rotationZ * rotationY * rotationX;
        
        // Compute scale
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(attributes.Size, 1.0f));

		// Compute transform matrix
        glm::mat4 transform = translation * rotation * scale;

		float texIndex = ComputeTextureIndex(attributes.Texture);

        // Extract custom UV bounds
        glm::vec2 uvMin = { attributes.TextureCoords.x, attributes.TextureCoords.y };
        glm::vec2 uvMax = { attributes.TextureCoords.z, attributes.TextureCoords.w };

        // Map UVs per corner (same order as QuadVertexPositions: bottom-left, bottom-right, top-right, top-left)
        glm::vec2 uvs[4] = 

        {
            { uvMin.x, uvMin.y },
            { uvMax.x, uvMin.y },
            { uvMax.x, uvMax.y },
            { uvMin.x, uvMax.y }
        };

        for (size_t i = 0; i < 4; i++)
        {
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color = attributes.TintColor;
            s_Data.QuadVertexBufferPtr->TexCoord = uvs[i];
            s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
            s_Data.QuadVertexBufferPtr++;
        }

        s_Data.QuadIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }

    void Renderer::DrawCube(const CubeAttributes& attributes)
    {
        LUMINA_ASSERT(s_Data.CubeVertexBufferPtr >= s_Data.CubeVertexBufferBase, "Vertex buffer pointer underflow");
        if (s_Data.CubeIndexCount >= MaxIndices)
        {
            EndBatch();
            StartBatch();
        }

        // Compute translation 
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), attributes.Position);

        // Compute rotation
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), attributes.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), attributes.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), attributes.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

        glm::mat4 rotation = rotationZ * rotationY * rotationX;

        // Compute scale
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), attributes.Size);

		// Compute transform matrix
        glm::mat4 transform = translation * rotation * scale;

        float texIndex = ComputeTextureIndex(attributes.Texture);

        // Extract custom UV bounds
        glm::vec2 uvMin = { attributes.TextureCoords.x, attributes.TextureCoords.y };
        glm::vec2 uvMax = { attributes.TextureCoords.z, attributes.TextureCoords.w };

        // Map UVs per corner (same order as QuadVertexPositions: bottom-left, bottom-right, top-right, top-left)
        glm::vec2 uvs[4] =
        {
            { uvMin.x, uvMin.y },
            { uvMax.x, uvMin.y },
            { uvMax.x, uvMax.y },
            { uvMin.x, uvMax.y }
        };

		for (size_t i = 0; i < 24; i++)
		{
			s_Data.CubeVertexBufferPtr->Position = transform * glm::vec4(s_Data.CubeVertexPositions[i], 1.0f);
			s_Data.CubeVertexBufferPtr->Color = attributes.TintColor;
			s_Data.CubeVertexBufferPtr->TexCoord = uvs[i % 4]; // Use modulo to cycle through UVs
			s_Data.CubeVertexBufferPtr->TexIndex = texIndex;
			s_Data.CubeVertexBufferPtr++;
		}

		s_Data.CubeIndexCount += 36;
		s_Data.Stats.CubeCount++;
    }

	Renderer::Statistics Renderer::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}
}