#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>

#include <stb_image_write.h>

#include "../Utils/FileReader.h"

#include "VertexArray.h"
#include "Buffer.h"
#include "BufferLayout.h"
#include "FrameBuffer.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <array>

namespace Lumina
{
    // Constants
    constexpr uint32_t MaxQuads = 10000;
    constexpr uint32_t MaxCubes = 10000;
    constexpr uint32_t MaxCircles = 10000;
    constexpr uint32_t MaxLines = 10000;

    constexpr uint32_t MaxVertices = MaxQuads * 4;
    constexpr uint32_t MaxIndices = MaxQuads * 6;
    constexpr uint32_t MaxTextureSlots = 32;

    // Vertex Structures
    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
    };

    struct CircleVertex
    {
        glm::vec3 WorldPosition;
		glm::vec3 LocalPosition;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        float Thickness;
        float Fade;
    };

    struct LineVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
    };

    struct GridVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        // Grid-specific data
        glm::vec3 GridPosition;
        glm::vec2 GridSize;
        float CellSize;
        glm::vec4 GridColor;
        float LineWidth;
        float ShowCheckerboard;
        glm::vec4 CheckerColor1;
        glm::vec4 CheckerColor2;
    };

    // Renderer data storage
    struct RendererData
    {
        // Targets
        Ref<RenderTarget> DefaultRenderTarget;
        Ref<RenderTarget> CurrentRenderTarget;

        // Quad
        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<IndexBuffer> QuadIndexBuffer;

        // Circle
        Ref<VertexArray> CircleVertexArray;
        Ref<VertexBuffer> CircleVertexBuffer;
        Ref<IndexBuffer> CircleIndexBuffer;

        // Line
        Ref<VertexArray> LineVertexArray;
        Ref<VertexBuffer> LineVertexBuffer;
        Ref<IndexBuffer> LineIndexBuffer;

        // Quad batch data
        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        // Circle batch data
        uint32_t CircleIndexCount = 0;
        CircleVertex* CircleVertexBufferBase = nullptr;
        CircleVertex* CircleVertexBufferPtr = nullptr;

        // Line batch data
        uint32_t LineVertexCount = 0; // Lines don't use indices
        LineVertex* LineVertexBufferBase = nullptr;
        LineVertex* LineVertexBufferPtr = nullptr;

        // Shaders
        Ref<ShaderProgram> QuadShader = nullptr;
        Ref<ShaderProgram> CubeShader = nullptr;
        Ref<ShaderProgram> CircleShader = nullptr;
        Ref<ShaderProgram> LineShader = nullptr;

        // Grid
        Ref<ShaderProgram> GridShader = nullptr;
        Ref<VertexArray> GridVertexArray;
        Ref<VertexBuffer> GridVertexBuffer;
        Ref<IndexBuffer> GridIndexBuffer;

        uint32_t GridIndexCount = 0;
        GridVertex* GridVertexBufferBase = nullptr;
        GridVertex* GridVertexBufferPtr = nullptr;

        // Texture Management
        std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; // 0 = white texture

        // Geometry Constants
        glm::vec4 QuadVertexPositions[4];
        glm::vec3 CircleVertexPositions[4]; 
        glm::vec2 TexCoords[4];

        // Geometry Variables
        float LineWidth = 3.0f;

        // Wireframe
        PolygonMode PolygonMode = PolygonMode::Fill;
        glm::vec3 WireFrameColor = { 0.0f, 1.0f, 0.0f };

        // View Projection
        glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);

        // Framebuffer Resolution
        uint32_t Width = 800;
        uint32_t Height = 600;

        // Statistics
        Renderer2D::Statistics Stats;
    };

    static RendererData s_Data;

    void Renderer2D::Init()
    {   
        LUMINA_LOG_INFO("Renderer2D: Initializing...");

		// Create default render target
        s_Data.DefaultRenderTarget = RenderTarget::Create(800, 600);
        s_Data.CurrentRenderTarget = s_Data.DefaultRenderTarget;

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
        s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };   // Bottom left
        s_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };   // Bottom right
        s_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };   // Top right
        s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };   // Top left

		// Set up default local position for circles
        s_Data.CircleVertexPositions[0] = { -1.0f, -1.0f, 0.0f },       // Bottom left
        s_Data.CircleVertexPositions[1] = {  1.0f, -1.0f, 0.0f };       // Bottom right
        s_Data.CircleVertexPositions[2] = {  1.0f,  1.0f, 0.0f };       // Top right
        s_Data.CircleVertexPositions[3] = { -1.0f,  1.0f, 0.0f };       // Top left

        // Set up default texture coordinates
        s_Data.TexCoords[0] = { 0.0f, 0.0f };  // Bottom left
        s_Data.TexCoords[1] = { 1.0f, 0.0f };  // Bottom right
        s_Data.TexCoords[2] = { 1.0f, 1.0f };  // Top right
        s_Data.TexCoords[3] = { 0.0f, 1.0f };  // Top left

        s_Data.CircleVertexArray = VertexArray::Create();
        s_Data.CircleVertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(CircleVertex));

        s_Data.CircleVertexBuffer->SetLayout({
            { BufferDataType::Float3, "a_WorldPosition" },
            { BufferDataType::Float3, "a_LocalPosition" },
            { BufferDataType::Float4, "a_Color" },
            { BufferDataType::Float2, "a_TexCoord" },
            { BufferDataType::Float,  "a_TexIndex" },
            { BufferDataType::Float,  "a_Thickness" },
            { BufferDataType::Float,  "a_Fade" }
        });

        s_Data.CircleVertexArray->SetVertexBuffer(s_Data.CircleVertexBuffer);

        // Use same quad-style index buffer
        s_Data.CircleIndexBuffer = IndexBuffer::Create(quadIndices.data(), MaxIndices);
        s_Data.CircleVertexArray->SetIndexBuffer(s_Data.CircleIndexBuffer);

        s_Data.CircleVertexBufferBase = new CircleVertex[MaxVertices];

        {
            std::string vertexSource = ReadFile("res/shaders/Circle.vert");
            std::string fragmentSource = ReadFile("res/shaders/Circle.frag");
            s_Data.CircleShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // -----------------------------
        // Line Initialization
        // -----------------------------
        s_Data.LineVertexArray = VertexArray::Create();
        s_Data.LineVertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(LineVertex));

        s_Data.LineVertexBuffer->SetLayout({
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float4, "a_Color" }
            });

        s_Data.LineVertexArray->SetVertexBuffer(s_Data.LineVertexBuffer);
        // Optional: Lines don’t use an index buffer unless you're batching special line topologies

        s_Data.LineVertexBufferBase = new LineVertex[MaxVertices];

        {
            std::string vertexSource = ReadFile("res/shaders/Line.vert");
            std::string fragmentSource = ReadFile("res/shaders/Line.frag");
            s_Data.LineShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Grid
        s_Data.GridVertexArray = VertexArray::Create();
        s_Data.GridVertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(GridVertex));

        s_Data.GridVertexBuffer->SetLayout({
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float4, "a_Color" },
            { BufferDataType::Float2, "a_TexCoord" },
            { BufferDataType::Float,  "a_TexIndex" },
            { BufferDataType::Float3, "a_GridPosition" },
            { BufferDataType::Float2, "a_GridSize" },
            { BufferDataType::Float,  "a_CellSize" },
            { BufferDataType::Float4, "a_GridColor" },
            { BufferDataType::Float,  "a_LineWidth" },
            { BufferDataType::Float,  "a_ShowCheckerboard" },
            { BufferDataType::Float4, "a_CheckerColor1" },
            { BufferDataType::Float4, "a_CheckerColor2" }
            });

        s_Data.GridVertexArray->SetVertexBuffer(s_Data.GridVertexBuffer);
        s_Data.GridIndexBuffer = IndexBuffer::Create(quadIndices.data(), MaxIndices);
        s_Data.GridVertexArray->SetIndexBuffer(s_Data.GridIndexBuffer);

        s_Data.GridVertexBufferBase = new GridVertex[MaxVertices];

        // Load grid shader
        {
            std::string vertexSource = ReadFile("res/shaders/Grid.vert");
            std::string fragmentSource = ReadFile("res/shaders/Grid.frag");
            s_Data.GridShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        LUMINA_LOG_INFO("Renderer2D: Initialization complete");
    }

    void Renderer2D::Shutdown()
    {
        LUMINA_LOG_INFO("Renderer2D: Shutting down...");
        
        // Explicitly reset all OpenGL resources while context is valid
        s_Data.DefaultRenderTarget.reset();
        s_Data.CurrentRenderTarget.reset();

        s_Data.QuadVertexArray.reset();
        s_Data.QuadVertexBuffer.reset();
        s_Data.QuadIndexBuffer.reset();

        s_Data.CircleVertexArray.reset();
        s_Data.CircleVertexBuffer.reset();
        s_Data.CircleIndexBuffer.reset();

        s_Data.LineVertexArray.reset();
        s_Data.LineVertexBuffer.reset();
        s_Data.LineIndexBuffer.reset();

        s_Data.GridVertexArray.reset();
        s_Data.GridVertexBuffer.reset();
        s_Data.GridIndexBuffer.reset();

        s_Data.QuadShader.reset();
        s_Data.CubeShader.reset();
        s_Data.CircleShader.reset();
        s_Data.LineShader.reset();
        s_Data.GridShader.reset();

        // Reset texture slots
        for (auto& texture : s_Data.TextureSlots) {
            texture.reset();
        }

        // Free ALL allocated memory
        delete[] s_Data.QuadVertexBufferBase;
        delete[] s_Data.CircleVertexBufferBase;  // Missing!
        delete[] s_Data.LineVertexBufferBase;    // Missing!
        delete[] s_Data.GridVertexBufferBase;    // Missing!

        // Set pointers to null for safety
        s_Data.QuadVertexBufferBase = nullptr;
        s_Data.CircleVertexBufferBase = nullptr;
        s_Data.LineVertexBufferBase = nullptr;
        s_Data.GridVertexBufferBase = nullptr;

        LUMINA_LOG_INFO("Renderer2D: Shutdown complete");
    }

    void Renderer2D::Begin(Camera& camera)
    {
        s_Data.ViewProjectionMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
        
        // We moved this up here because we want the screen to only be cleared when all things have been drawn
		// Since we bind on begin, we need to add checks to ensure that the user cant switch targets in the middle of a batch
        s_Data.CurrentRenderTarget->Bind();
        s_Data.CurrentRenderTarget->Resize(s_Data.Width, s_Data.Height);

        RenderCommands::Clear();
        RenderCommands::SetViewport(0, 0, s_Data.Width, s_Data.Height);
        RenderCommands::EnableDepthTest();
        RenderCommands::SetPolygonMode(s_Data.PolygonMode);

        StartBatch(); 
    }

    void Renderer2D::Begin(glm::mat4& viewProjection)
    {
        s_Data.ViewProjectionMatrix = viewProjection;
        StartBatch();
    }

    void Renderer2D::End()
    {
		EndBatch();

        s_Data.CurrentRenderTarget->Unbind();
    }

    void Renderer2D::StartBatch()
    {
        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.CircleIndexCount = 0;
        s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

        s_Data.LineVertexCount = 0;
        s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;

        s_Data.GridIndexCount = 0;
        s_Data.GridVertexBufferPtr = s_Data.GridVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::EndBatch()
    {
        bool issueDraw = false;

        // --- QUADS ---
        uint32_t quadDataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
        if (quadDataSize > 0)
        {
            s_Data.Stats.DataSize += quadDataSize;
            s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, quadDataSize);
            issueDraw = true;
        }

        // --- CIRCLES ---
        uint32_t circleDataSize = (uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase;
        if (circleDataSize > 0)
        {
            s_Data.Stats.DataSize += circleDataSize;
            s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, circleDataSize);
            issueDraw = true;
        }

        // --- LINES ---
        uint32_t lineDataSize = (uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase;
        if (lineDataSize > 0)
        {
            s_Data.Stats.DataSize += lineDataSize;
            s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, lineDataSize);
            issueDraw = true;
        }

        // --- GRIDS ---
        uint32_t gridDataSize = (uint8_t*)s_Data.GridVertexBufferPtr - (uint8_t*)s_Data.GridVertexBufferBase;
        if (gridDataSize > 0)
        {
            s_Data.Stats.DataSize += gridDataSize;
            s_Data.GridVertexBuffer->SetData(s_Data.GridVertexBufferBase, gridDataSize);
            issueDraw = true;
        }

        if (issueDraw)
        {
            Flush();
        }
    }

    void Renderer2D::Flush()
    {
        // Bind all textures
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
            s_Data.TextureSlots[i]->Bind(i);

        // Draw Quads
        if (s_Data.QuadIndexCount > 0)
        {
            s_Data.QuadShader->Bind();
            s_Data.QuadShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
            s_Data.QuadShader->SetUniformInt("u_WireframeMode", (int)s_Data.PolygonMode);
            s_Data.QuadShader->SetUniformVec3("u_WireframeColor", s_Data.WireFrameColor);

            s_Data.QuadVertexArray->Bind();
			RenderCommands::DrawElementsWithCount(s_Data.QuadVertexArray, PrimitiveType::Triangles, s_Data.QuadIndexCount);
            s_Data.QuadVertexArray->Unbind();

            s_Data.QuadShader->Unbind();

            s_Data.Stats.DrawCalls++;
        }

        // Draw Circles
        if (s_Data.CircleIndexCount > 0)
        {
            s_Data.CircleShader->Bind();
            s_Data.CircleShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
            s_Data.CircleShader->SetUniformInt("u_WireframeMode", (int)s_Data.PolygonMode);
            s_Data.CircleShader->SetUniformVec3("u_WireframeColor", s_Data.WireFrameColor);

            s_Data.CircleVertexArray->Bind();
			RenderCommands::DrawElementsWithCount(s_Data.CircleVertexArray, PrimitiveType::Triangles, s_Data.CircleIndexCount);
            s_Data.CircleVertexArray->Unbind();

            s_Data.CircleShader->Unbind();

            s_Data.Stats.DrawCalls++;
        }

        // Draw Lines
        if (s_Data.LineVertexCount > 0)
        {
            s_Data.LineShader->Bind();
            s_Data.LineShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
            s_Data.LineShader->SetUniformInt("u_WireframeMode", (int)s_Data.PolygonMode);
            s_Data.LineShader->SetUniformVec3("u_WireframeColor", s_Data.WireFrameColor);

            s_Data.LineVertexArray->Bind();
			RenderCommands::SetLineWidth(s_Data.LineWidth);
            RenderCommands::DrawLines(s_Data.LineVertexArray, s_Data.LineVertexCount);
            s_Data.LineVertexArray->Unbind();

            s_Data.LineShader->Unbind();
            
            s_Data.Stats.DrawCalls++;
        }

        if (s_Data.GridIndexCount > 0)
        {
            s_Data.GridShader->Bind();
            s_Data.GridShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);

            s_Data.GridVertexArray->Bind();
            RenderCommands::DrawElementsWithCount(s_Data.GridVertexArray, PrimitiveType::Triangles, s_Data.GridIndexCount);
            s_Data.GridVertexArray->Unbind();

            s_Data.GridShader->Unbind();
            s_Data.Stats.DrawCalls++;
        }

        // Unbind all textures
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
            s_Data.TextureSlots[i]->Unbind();

        // Update stats
        s_Data.Stats.TexturesUsed = s_Data.TextureSlotIndex - 1;

        // Reset counts after flush
        s_Data.QuadIndexCount = 0;
        s_Data.CircleIndexCount = 0;
        s_Data.LineVertexCount = 0;
		s_Data.GridIndexCount = 0;
    }


    void Renderer2D::SetResolution(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0) 
        {
            std::cerr << "Invalid resolution: " << width << "x" << height << std::endl;
            return;
        }

        s_Data.Width = width;
        s_Data.Height = height;
    }

    uint32_t Renderer2D::GetImage()
    {
        return s_Data.CurrentRenderTarget->GetTexture();
    }

    void Renderer2D::SetRenderMode(PolygonMode mode)
    {
        s_Data.PolygonMode = mode; 
    }

	float Renderer2D::ComputeTextureIndex(const Ref<Texture>& texture)
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

    glm::vec2 Renderer2D::GetResolution()
    {
        return { s_Data.Width, s_Data.Height };
    }

    void Renderer2D::DrawQuad(const QuadAttributes& attributes)
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
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(attributes.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(attributes.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(attributes.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

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

    void Renderer2D::DrawCircle(const CircleAttributes& attributes)
    {
        LUMINA_ASSERT(s_Data.CircleVertexBufferPtr >= s_Data.CircleVertexBufferBase, "Vertex buffer pointer underflow");

        if (s_Data.CircleIndexCount >= MaxIndices)
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

        // Compute scale - IMPORTANT: Use attributes.Radius for both X and Y to maintain circle shape
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(attributes.Radius.x, attributes.Radius.y, 1.0f));

        // Compute transform matrix
        glm::mat4 transform = translation * rotation * scale;

		// Compute texture index (0 = white texture)
        float texIndex = ComputeTextureIndex(attributes.Texture);

        // Extract custom UV bounds
        glm::vec2 uvMin = { attributes.TextureCoords.x, attributes.TextureCoords.y };
        glm::vec2 uvMax = { attributes.TextureCoords.z, attributes.TextureCoords.w };

        // Map UVs
        glm::vec2 uvs[4] =
        {
            { uvMin.x, uvMin.y },
            { uvMax.x, uvMin.y },
            { uvMax.x, uvMax.y },
            { uvMin.x, uvMax.y }
        };

		// Fill vertex buffer with circle vertices
        for (size_t i = 0; i < 4; i++)
        {
            s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
            s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.CircleVertexPositions[i];
            s_Data.CircleVertexBufferPtr->Color = attributes.Color;
            s_Data.CircleVertexBufferPtr->TexCoord = uvs[i];
            s_Data.CircleVertexBufferPtr->TexIndex = texIndex;
            s_Data.CircleVertexBufferPtr->Thickness = attributes.Thickness;
            s_Data.CircleVertexBufferPtr->Fade = attributes.Fade;
            s_Data.CircleVertexBufferPtr++;
        }

        s_Data.CircleIndexCount += 6;
        s_Data.Stats.CircleCount++;
    }

    void Renderer2D::DrawLine(const LineAttributes& attributes)
    {
        if (s_Data.LineVertexCount >= MaxVertices)
        {
            EndBatch();
            StartBatch();
        }

        if (s_Data.LineWidth != attributes.Thickness)
        {
            EndBatch();
            StartBatch();
			s_Data.LineWidth = attributes.Thickness;
        }

        s_Data.LineVertexBufferPtr->Position = attributes.Start;
        s_Data.LineVertexBufferPtr->Color = attributes.Color;
        s_Data.LineVertexBufferPtr++;
        s_Data.LineVertexCount++;

        s_Data.LineVertexBufferPtr->Position = attributes.End;
        s_Data.LineVertexBufferPtr->Color = attributes.Color;
        s_Data.LineVertexBufferPtr++;
        s_Data.LineVertexCount++;

        s_Data.Stats.LineCount++;
    }

    void Renderer2D::DrawGrid(const GridAttributes& attributes)
    {
        if (s_Data.GridIndexCount >= MaxIndices)
        {
            EndBatch();
            StartBatch();
        }

        // Calculate transform matrix
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), attributes.Position);
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(attributes.Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(attributes.Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(attributes.Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 rotation = rotationZ * rotationY * rotationX;
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(attributes.Size, 1.0f));
        glm::mat4 transform = translation * rotation * scale;

        // Create vertices for the grid quad
        glm::vec2 texCoords[4] = {
            { 0.0f, 0.0f }, // Bottom left
            { 1.0f, 0.0f }, // Bottom right
            { 1.0f, 1.0f }, // Top right
            { 0.0f, 1.0f }  // Top left
        };

        for (size_t i = 0; i < 4; i++)
        {
            s_Data.GridVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.GridVertexBufferPtr->Color = attributes.Color;
            s_Data.GridVertexBufferPtr->TexCoord = texCoords[i];
            s_Data.GridVertexBufferPtr->TexIndex = 0.0f;

            // Grid-specific data (same for all vertices of this grid)
            s_Data.GridVertexBufferPtr->GridPosition = attributes.Position;
            s_Data.GridVertexBufferPtr->GridSize = attributes.Size;
            s_Data.GridVertexBufferPtr->CellSize = attributes.GridSize;
            s_Data.GridVertexBufferPtr->GridColor = attributes.Color;
            s_Data.GridVertexBufferPtr->LineWidth = attributes.LineWidth;
            s_Data.GridVertexBufferPtr->ShowCheckerboard = attributes.ShowCheckerboard ? 1.0f : 0.0f;
            s_Data.GridVertexBufferPtr->CheckerColor1 = attributes.CheckerColor1;
            s_Data.GridVertexBufferPtr->CheckerColor2 = attributes.CheckerColor2;

            s_Data.GridVertexBufferPtr++;
        }

        s_Data.GridIndexCount += 6;
        s_Data.Stats.QuadCount++; // Or add GridCount to stats
    }

    void Renderer2D::SetRenderTarget(Ref<RenderTarget> target)
    {
        if (target)
        {
            s_Data.CurrentRenderTarget = target;
        }
        else
        {
            s_Data.CurrentRenderTarget = s_Data.DefaultRenderTarget;
        }
    }

    void Renderer2D::SetRenderTarget(std::nullptr_t)
    {
        s_Data.CurrentRenderTarget = s_Data.DefaultRenderTarget;
    }

    Ref<RenderTarget> Renderer2D::GetCurrentRenderTarget()
    {
        return s_Data.CurrentRenderTarget;
    }

    Ref<RenderTarget> Renderer2D::CreateRenderTarget(uint32_t width, uint32_t height)
    {
        return RenderTarget::Create(width, height);
    }

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data.Stats;
	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data.Stats, 0, sizeof(Statistics));
	}
}