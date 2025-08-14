#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <stb_image_write.h>
#include "../Utils/FileReader.h"
#include "VertexArray.h"
#include "Buffer.h"
#include "BufferLayout.h"
#include "FrameBuffer.h"

#include "Fonts/DefaultFont.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <array>

namespace Lumina
{
    constexpr uint32_t MaxQuads = 10000;
    constexpr uint32_t MaxCircles = 10000;
    constexpr uint32_t MaxLines = 10000;
    constexpr uint32_t MaxText = 10000;
    constexpr uint32_t MaxPixels = 50000;
    constexpr uint32_t MaxTriangles = 10000;
    constexpr uint32_t MaxGrids = 1000;
	constexpr uint32_t MaxPointLights = 32;

    constexpr uint32_t MaxVertices = MaxQuads * 4;
    constexpr uint32_t MaxIndices = MaxQuads * 6;
    constexpr uint32_t MaxTextureSlots = 32;

	constexpr uint32_t BindingLocationPointLight = 1;

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

    struct TextVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
    };

    struct PixelVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
    };

    struct TriangleVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
    };

    struct GridVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexIndex;
        glm::vec3 GridPosition;
        glm::vec2 GridSize;
        float CellSize;
        glm::vec4 GridColor;
        float LineWidth;
        float ShowCheckerboard;
        glm::vec4 CheckerColor1;
        glm::vec4 CheckerColor2;
    };

    struct PointLight
    {
        glm::vec3 Position;
		float Intensity;
		glm::vec3 Color;
		float Radius;
        float BlendingMode;
        float Falloff;
		float Unused0;
		float Unused1;
    };

    struct RendererData
    {
        Ref<RenderTarget> DefaultRenderTarget;
        Ref<RenderTarget> CurrentRenderTarget;

        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<IndexBuffer> QuadIndexBuffer;

        Ref<VertexArray> CircleVertexArray;
        Ref<VertexBuffer> CircleVertexBuffer;
        Ref<IndexBuffer> CircleIndexBuffer;

        Ref<VertexArray> LineVertexArray;
        Ref<VertexBuffer> LineVertexBuffer;

        Ref<VertexArray> TextVertexArray;
        Ref<VertexBuffer> TextVertexBuffer;
        Ref<IndexBuffer> TextIndexBuffer;

        Ref<VertexArray> PixelVertexArray;
        Ref<VertexBuffer> PixelVertexBuffer;

        Ref<VertexArray> TriangleVertexArray;
        Ref<VertexBuffer> TriangleVertexBuffer;

        Ref<VertexArray> GridVertexArray;
        Ref<VertexBuffer> GridVertexBuffer;
        Ref<IndexBuffer> GridIndexBuffer;

		Ref<UniformBuffer> PointLightUniformBuffer;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        uint32_t CircleIndexCount = 0;
        CircleVertex* CircleVertexBufferBase = nullptr;
        CircleVertex* CircleVertexBufferPtr = nullptr;

        uint32_t LineVertexCount = 0;
        LineVertex* LineVertexBufferBase = nullptr;
        LineVertex* LineVertexBufferPtr = nullptr;

        uint32_t TextIndexCount = 0;
        TextVertex* TextVertexBufferBase = nullptr;
        TextVertex* TextVertexBufferPtr = nullptr;

        uint32_t PixelVertexCount = 0;
        PixelVertex* PixelVertexBufferBase = nullptr;
        PixelVertex* PixelVertexBufferPtr = nullptr;

        uint32_t TriangleVertexCount = 0;
        TriangleVertex* TriangleVertexBufferBase = nullptr;
        TriangleVertex* TriangleVertexBufferPtr = nullptr;

        uint32_t GridIndexCount = 0;
        GridVertex* GridVertexBufferBase = nullptr;
        GridVertex* GridVertexBufferPtr = nullptr;

        uint32_t PointLightCount = 0;
		PointLight* PointLightUniformBufferBase = nullptr;
		PointLight* PointLightUniformBufferPtr = nullptr;

        Ref<ShaderProgram> QuadShader = nullptr;
        Ref<ShaderProgram> CircleShader = nullptr;
        Ref<ShaderProgram> LineShader = nullptr;
        Ref<ShaderProgram> TextShader = nullptr;
        Ref<ShaderProgram> PixelShader = nullptr;
        Ref<ShaderProgram> TriangleShader = nullptr;
        Ref<ShaderProgram> GridShader = nullptr;

        std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1;

        glm::vec4 QuadVertexPositions[4];
        glm::vec3 CircleVertexPositions[4];
        glm::vec2 TexCoords[4];

        float LineWidth = 3.0f;
        Ref<Texture> DefaultFont = nullptr;

        PolygonMode PolygonMode = PolygonMode::Fill;
        glm::vec3 WireFrameColor = { 0.0f, 1.0f, 0.0f };

        glm::mat4 ViewProjectionMatrix = glm::mat4(1.0f);

        uint32_t Width = 800;
        uint32_t Height = 600;

        Renderer2D::Statistics Stats;

        bool UseLighting = false;
		glm::vec3 AmbientColor = { 0.1f, 0.1f, 0.1f };
		float AmbientIntensity = 0.5f;

        glm::vec3 QuadPosition = { 0.0f, 0.0f, 0.0f };
        glm::vec3 QuadRotation = { 0.0f, 0.0f, 0.0f };
        glm::vec2 QuadSize = { 1.0f, 1.0f };
        Ref<Texture> QuadTexture = nullptr;
        glm::vec4 QuadTextureCoords = { 0.0f, 0.0f, 1.0f, 1.0f };
        glm::vec4 QuadTintColor = { 1.0f, 1.0f, 1.0f, 1.0f };

        glm::vec3 CirclePosition = { 0.0f, 0.0f, 0.0f };
        glm::vec3 CircleRotation = { 0.0f, 0.0f, 0.0f };
        glm::vec2 CircleRadius = { 1.0f, 1.0f };
        Ref<Texture> CircleTexture = nullptr;
        glm::vec4 CircleTextureCoords = { 0.0f, 0.0f, 1.0f, 1.0f };
        glm::vec4 CircleColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        float CircleThickness = 1.0f;
        float CircleFade = 0.0f;

        glm::vec3 LineStart = { 0.0f, 0.0f, 0.0f };
        glm::vec3 LineEnd = { 1.0f, 1.0f, 0.0f };
        float LineThickness = 2.0f;
        glm::vec4 LineColor = { 1.0f, 1.0f, 1.0f, 1.0f };

        std::string TextContent = "";
        glm::vec3 TextPosition = { 0.0f, 0.0f, 0.0f };
        glm::vec4 TextColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        float TextSize = 1.0f;
        Ref<Texture> TextFont = nullptr;

        glm::vec3 PixelPosition = { 0.0f, 0.0f, 0.0f };
        glm::vec4 PixelColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		float PixelSize = 1.0f;

        glm::vec3 TrianglePoint1 = { 0.0f, 0.0f, 0.0f };
        glm::vec3 TrianglePoint2 = { 1.0f, 0.0f, 0.0f };
        glm::vec3 TrianglePoint3 = { 0.5f, 1.0f, 0.0f };
        Ref<Texture> TriangleTexture = nullptr;
        glm::vec4 TriangleColor = { 1.0f, 1.0f, 1.0f, 1.0f };

        glm::vec3 GridPosition = { 0.0f, 0.0f, 0.0f };
        glm::vec3 GridRotation = { 0.0f, 0.0f, 0.0f };
        glm::vec2 GridSize = { 1000.0f, 1000.0f };
        float GridCellSize = 1.0f;
        glm::vec4 GridColor = { 0.3f, 0.3f, 0.3f, 0.8f };
        float GridLineWidth = 1.0f;
        bool GridShowCheckerboard = true;
        glm::vec4 GridCheckerColor1 = { 0.9f, 0.9f, 0.9f, 0.2f };
        glm::vec4 GridCheckerColor2 = { 0.8f, 0.8f, 0.8f, 0.2f };

		glm::vec3 PointLightPosition = { 0.0f, 0.0f, 0.0f };
		float PointLightIntensity = 1.0f;
		glm::vec3 PointLightColor = { 1.0f, 1.0f, 1.0f };
		float PointLightRadius = 5.0f;
		LightBlendMode PointLightBlendMode = LightBlendMode::Additive;
		float PointLightFalloff = 1.0f;
    };

    static RendererData s_Data;

    void Renderer2D::Init()
    {
        LUMINA_LOG_INFO("Renderer2D: Initializing...");

        s_Data.DefaultRenderTarget = RenderTarget::Create(800, 600);
        s_Data.CurrentRenderTarget = s_Data.DefaultRenderTarget;

        // Quad setup
        s_Data.QuadVertexArray = VertexArray::Create();
        s_Data.QuadVertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(QuadVertex));
        s_Data.QuadVertexBuffer->SetLayout({
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float4, "a_Color" },
            { BufferDataType::Float2, "a_TexCoord" },
            { BufferDataType::Float,  "a_TexIndex" }
            });
        s_Data.QuadVertexArray->SetVertexBuffer(s_Data.QuadVertexBuffer);

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
        s_Data.QuadVertexBufferBase = new QuadVertex[MaxVertices];

        {
            std::string vertexSource = ReadFile("res/shaders/Quad.vert");
            std::string fragmentSource = ReadFile("res/shaders/Quad.frag");
            s_Data.QuadShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Circle setup
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
        s_Data.CircleIndexBuffer = IndexBuffer::Create(quadIndices.data(), MaxIndices);
        s_Data.CircleVertexArray->SetIndexBuffer(s_Data.CircleIndexBuffer);
        s_Data.CircleVertexBufferBase = new CircleVertex[MaxVertices];

        {
            std::string vertexSource = ReadFile("res/shaders/Circle.vert");
            std::string fragmentSource = ReadFile("res/shaders/Circle.frag");
            s_Data.CircleShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Line setup
        s_Data.LineVertexArray = VertexArray::Create();
        s_Data.LineVertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(LineVertex));
        s_Data.LineVertexBuffer->SetLayout({
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float4, "a_Color" }
            });
        s_Data.LineVertexArray->SetVertexBuffer(s_Data.LineVertexBuffer);
        s_Data.LineVertexBufferBase = new LineVertex[MaxVertices];

        {
            std::string vertexSource = ReadFile("res/shaders/Line.vert");
            std::string fragmentSource = ReadFile("res/shaders/Line.frag");
            s_Data.LineShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Text setup
        s_Data.TextVertexArray = VertexArray::Create();
        s_Data.TextVertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(TextVertex));
        s_Data.TextVertexBuffer->SetLayout({
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float4, "a_Color" },
            { BufferDataType::Float2, "a_TexCoord" },
            { BufferDataType::Float,  "a_TexIndex" }
            });
        s_Data.TextVertexArray->SetVertexBuffer(s_Data.TextVertexBuffer);
        s_Data.TextIndexBuffer = IndexBuffer::Create(quadIndices.data(), MaxIndices);
        s_Data.TextVertexArray->SetIndexBuffer(s_Data.TextIndexBuffer);
        s_Data.TextVertexBufferBase = new TextVertex[MaxVertices];

        {
            std::string vertexSource = ReadFile("res/shaders/Text.vert");
            std::string fragmentSource = ReadFile("res/shaders/Text.frag");
            s_Data.TextShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Pixel setup
        s_Data.PixelVertexArray = VertexArray::Create();
        s_Data.PixelVertexBuffer = VertexBuffer::Create(MaxPixels * sizeof(PixelVertex));
        s_Data.PixelVertexBuffer->SetLayout({
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float4, "a_Color" }
            });
        s_Data.PixelVertexArray->SetVertexBuffer(s_Data.PixelVertexBuffer);
        s_Data.PixelVertexBufferBase = new PixelVertex[MaxPixels];

        {
            std::string vertexSource = ReadFile("res/shaders/Pixel.vert");
            std::string fragmentSource = ReadFile("res/shaders/Pixel.frag");
            s_Data.PixelShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Triangle setup
        s_Data.TriangleVertexArray = VertexArray::Create();
        s_Data.TriangleVertexBuffer = VertexBuffer::Create(MaxTriangles * 3 * sizeof(TriangleVertex));
        s_Data.TriangleVertexBuffer->SetLayout({
            { BufferDataType::Float3, "a_Position" },
            { BufferDataType::Float4, "a_Color" },
            { BufferDataType::Float2, "a_TexCoord" },
            { BufferDataType::Float,  "a_TexIndex" }
            });
        s_Data.TriangleVertexArray->SetVertexBuffer(s_Data.TriangleVertexBuffer);
        s_Data.TriangleVertexBufferBase = new TriangleVertex[MaxTriangles * 3];

        {
            std::string vertexSource = ReadFile("res/shaders/Triangle.vert");
            std::string fragmentSource = ReadFile("res/shaders/Triangle.frag");
            s_Data.TriangleShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        // Grid setup
        s_Data.GridVertexArray = VertexArray::Create();
        s_Data.GridVertexBuffer = VertexBuffer::Create(MaxGrids * 4 * sizeof(GridVertex));
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
        s_Data.GridVertexBufferBase = new GridVertex[MaxGrids * 4];

        {
            std::string vertexSource = ReadFile("res/shaders/Grid.vert");
            std::string fragmentSource = ReadFile("res/shaders/Grid.frag");
            s_Data.GridShader = ShaderProgram::Create(vertexSource, fragmentSource);
        }

        s_Data.PointLightUniformBuffer = UniformBuffer::Create(sizeof(PointLight) * MaxPointLights, BufferUsage::Dynamic);
        s_Data.PointLightUniformBufferBase = new PointLight[MaxPointLights];
        s_Data.PointLightUniformBufferPtr = s_Data.PointLightUniformBufferBase;

        uint32_t whiteTextureData = 0xffffffff;
        s_Data.TextureSlots[0] = Texture::Create(1, 1);
        s_Data.TextureSlots[0]->SetData(&whiteTextureData, sizeof(uint32_t));

		s_Data.DefaultFont = DefaultFont::Create();


        s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

        s_Data.CircleVertexPositions[0] = { -1.0f, -1.0f, 0.0f };
        s_Data.CircleVertexPositions[1] = { 1.0f, -1.0f, 0.0f };
        s_Data.CircleVertexPositions[2] = { 1.0f,  1.0f, 0.0f };
        s_Data.CircleVertexPositions[3] = { -1.0f,  1.0f, 0.0f };

        s_Data.TexCoords[0] = { 0.0f, 0.0f };
        s_Data.TexCoords[1] = { 1.0f, 0.0f };
        s_Data.TexCoords[2] = { 1.0f, 1.0f };
        s_Data.TexCoords[3] = { 0.0f, 1.0f };

        LUMINA_LOG_INFO("Renderer2D: Initialization complete");
    }

    void Renderer2D::Shutdown()
    {
        LUMINA_LOG_INFO("Renderer2D: Shutting down...");

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
        s_Data.TextVertexArray.reset();
        s_Data.TextVertexBuffer.reset();
        s_Data.TextIndexBuffer.reset();
        s_Data.PixelVertexArray.reset();
        s_Data.PixelVertexBuffer.reset();
        s_Data.TriangleVertexArray.reset();
        s_Data.TriangleVertexBuffer.reset();
        s_Data.GridVertexArray.reset();
        s_Data.GridVertexBuffer.reset();
        s_Data.GridIndexBuffer.reset();
        s_Data.PointLightUniformBuffer.reset();

        s_Data.QuadShader.reset();
        s_Data.CircleShader.reset();
        s_Data.LineShader.reset();
        s_Data.TextShader.reset();
        s_Data.PixelShader.reset();
        s_Data.TriangleShader.reset();
        s_Data.GridShader.reset();

        for (auto& texture : s_Data.TextureSlots) {
            texture.reset();
        }

        delete[] s_Data.QuadVertexBufferBase;
        delete[] s_Data.CircleVertexBufferBase;
        delete[] s_Data.LineVertexBufferBase;
        delete[] s_Data.TextVertexBufferBase;
        delete[] s_Data.PixelVertexBufferBase;
        delete[] s_Data.TriangleVertexBufferBase;
        delete[] s_Data.GridVertexBufferBase;
        delete[] s_Data.PointLightUniformBufferBase;

        s_Data.QuadVertexBufferBase = nullptr;
        s_Data.CircleVertexBufferBase = nullptr;
        s_Data.LineVertexBufferBase = nullptr;
        s_Data.TextVertexBufferBase = nullptr;
        s_Data.PixelVertexBufferBase = nullptr;
        s_Data.TriangleVertexBufferBase = nullptr;
        s_Data.GridVertexBufferBase = nullptr;
		s_Data.PointLightUniformBufferBase = nullptr;
		
        LUMINA_LOG_INFO("Renderer2D: Shutdown complete");
    }

    void Renderer2D::Begin(Camera& camera)
    {
        s_Data.ViewProjectionMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();

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

        s_Data.TextIndexCount = 0;
        s_Data.TextVertexBufferPtr = s_Data.TextVertexBufferBase;

        s_Data.PixelVertexCount = 0;
        s_Data.PixelVertexBufferPtr = s_Data.PixelVertexBufferBase;

        s_Data.TriangleVertexCount = 0;
        s_Data.TriangleVertexBufferPtr = s_Data.TriangleVertexBufferBase;

        s_Data.GridIndexCount = 0;
        s_Data.GridVertexBufferPtr = s_Data.GridVertexBufferBase;

		s_Data.PointLightCount = 0;
		s_Data.PointLightUniformBufferPtr = s_Data.PointLightUniformBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::EndBatch()
    {
        bool issueDraw = false;

        uint32_t quadDataSize = (uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase;
        if (quadDataSize > 0)
        {
            s_Data.Stats.DataSize += quadDataSize;
            s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, quadDataSize);
            issueDraw = true;
        }

        uint32_t circleDataSize = (uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase;
        if (circleDataSize > 0)
        {
            s_Data.Stats.DataSize += circleDataSize;
            s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, circleDataSize);
            issueDraw = true;
        }

        uint32_t lineDataSize = (uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase;
        if (lineDataSize > 0)
        {
            s_Data.Stats.DataSize += lineDataSize;
            s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, lineDataSize);
            issueDraw = true;
        }

        uint32_t textDataSize = (uint8_t*)s_Data.TextVertexBufferPtr - (uint8_t*)s_Data.TextVertexBufferBase;
        if (textDataSize > 0)
        {
            s_Data.Stats.DataSize += textDataSize;
            s_Data.TextVertexBuffer->SetData(s_Data.TextVertexBufferBase, textDataSize);
            issueDraw = true;
        }

        uint32_t pixelDataSize = (uint8_t*)s_Data.PixelVertexBufferPtr - (uint8_t*)s_Data.PixelVertexBufferBase;
        if (pixelDataSize > 0)
        {
            s_Data.Stats.DataSize += pixelDataSize;
            s_Data.PixelVertexBuffer->SetData(s_Data.PixelVertexBufferBase, pixelDataSize);
            issueDraw = true;
        }

        uint32_t triangleDataSize = (uint8_t*)s_Data.TriangleVertexBufferPtr - (uint8_t*)s_Data.TriangleVertexBufferBase;
        if (triangleDataSize > 0)
        {
            s_Data.Stats.DataSize += triangleDataSize;
            s_Data.TriangleVertexBuffer->SetData(s_Data.TriangleVertexBufferBase, triangleDataSize);
            issueDraw = true;
        }

        uint32_t gridDataSize = (uint8_t*)s_Data.GridVertexBufferPtr - (uint8_t*)s_Data.GridVertexBufferBase;
        if (gridDataSize > 0)
        {
            s_Data.Stats.DataSize += gridDataSize;
            s_Data.GridVertexBuffer->SetData(s_Data.GridVertexBufferBase, gridDataSize);
            issueDraw = true;
        }

        if (s_Data.PointLightCount > 0)
        {
            uint32_t lightDataSize = s_Data.PointLightCount * sizeof(PointLight);
            s_Data.PointLightUniformBuffer->SetData(s_Data.PointLightUniformBufferBase, lightDataSize);
        }

        if (issueDraw)
        {
            Flush();
        }
    }

    void Renderer2D::Flush()
    {
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
            s_Data.TextureSlots[i]->Bind(i);

        if (s_Data.PointLightCount > 0 && s_Data.UseLighting)
            s_Data.PointLightUniformBuffer->Bind(BindingLocationPointLight);
        
        if (s_Data.QuadIndexCount > 0)
        {
            s_Data.QuadShader->Bind();
            s_Data.QuadShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
            s_Data.QuadShader->SetUniformInt("u_WireframeMode", (int)s_Data.PolygonMode);
            s_Data.QuadShader->SetUniformVec3("u_WireframeColor", s_Data.WireFrameColor);
			s_Data.QuadShader->SetUniformInt("u_EnableLighting", (int)s_Data.UseLighting);
			s_Data.QuadShader->SetUniformVec3("u_AmbientColor", s_Data.AmbientColor);
            s_Data.QuadShader->SetUniformFloat("u_AmbientIntensity", s_Data.AmbientIntensity);
            s_Data.QuadShader->SetUniformInt("u_PointLightCount", (int)s_Data.PointLightCount);

            s_Data.QuadVertexArray->Bind();
            RenderCommands::DrawElementsWithCount(s_Data.QuadVertexArray, PrimitiveType::Triangles, s_Data.QuadIndexCount);
            s_Data.QuadVertexArray->Unbind();
            s_Data.QuadShader->Unbind();
            s_Data.Stats.DrawCalls++;
        }

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

        if (s_Data.TextIndexCount > 0)
        {
            s_Data.TextShader->Bind();
            s_Data.TextShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
            s_Data.TextShader->SetUniformInt("u_WireframeMode", (int)s_Data.PolygonMode);
            s_Data.TextShader->SetUniformVec3("u_WireframeColor", s_Data.WireFrameColor);

            s_Data.TextVertexArray->Bind();
            RenderCommands::DrawElementsWithCount(s_Data.TextVertexArray, PrimitiveType::Triangles, s_Data.TextIndexCount);
            s_Data.TextVertexArray->Unbind();
            s_Data.TextShader->Unbind();
            s_Data.Stats.DrawCalls++;
        }

        if (s_Data.PixelVertexCount > 0)
        {
            s_Data.PixelShader->Bind();
            s_Data.PixelShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);

            s_Data.PixelVertexArray->Bind();
			RenderCommands::SetPointSize(s_Data.PixelSize);
            RenderCommands::DrawPoints(s_Data.PixelVertexArray, s_Data.PixelVertexCount);
			RenderCommands::SetPointSize(1.0f);
            s_Data.PixelVertexArray->Unbind();
            s_Data.PixelShader->Unbind();
            s_Data.Stats.DrawCalls++;
        }

        if (s_Data.TriangleVertexCount > 0)
        {
            s_Data.TriangleShader->Bind();
            s_Data.TriangleShader->SetUniformMat4("u_ViewProjection", s_Data.ViewProjectionMatrix);
            s_Data.TriangleShader->SetUniformInt("u_WireframeMode", (int)s_Data.PolygonMode);
            s_Data.TriangleShader->SetUniformVec3("u_WireframeColor", s_Data.WireFrameColor);

            s_Data.TriangleVertexArray->Bind();
            RenderCommands::DrawArrays(s_Data.TriangleVertexArray, PrimitiveType::Triangles, s_Data.TriangleVertexCount);
            s_Data.TriangleVertexArray->Unbind();
            s_Data.TriangleShader->Unbind();
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

        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
            s_Data.TextureSlots[i]->Unbind();

        if (s_Data.PointLightCount > 0 && s_Data.UseLighting) 
            s_Data.PointLightUniformBuffer->Unbind();

        s_Data.Stats.TexturesUsed = s_Data.TextureSlotIndex - 1;

        s_Data.QuadIndexCount = 0;
        s_Data.CircleIndexCount = 0;
        s_Data.LineVertexCount = 0;
        s_Data.TextIndexCount = 0;
        s_Data.PixelVertexCount = 0;
        s_Data.TriangleVertexCount = 0;
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

    glm::vec2 Renderer2D::GetResolution()
    {
        return { s_Data.Width, s_Data.Height };
    }

    void Renderer2D::SetRenderMode(PolygonMode mode)
    {
        s_Data.PolygonMode = mode;
    }

    uint32_t Renderer2D::GetImage()
    {
        return s_Data.CurrentRenderTarget->GetTexture();
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

    void Renderer2D::UseLighting(bool enabled)
    {
        s_Data.UseLighting = enabled;
        s_Data.Stats.LightingUsed = enabled;
    }

    bool Renderer2D::IsLightingUsed()
    {
        return s_Data.UseLighting;
    }

    void Renderer2D::SetAmbientLightColor(const glm::vec3& color)
    {
        s_Data.AmbientColor = color;
    }

    void Renderer2D::SetAmbientLightIntensity(float intensity)
    {
        s_Data.AmbientIntensity = intensity;
    }

    void Renderer2D::SetQuadPosition(const glm::vec3& position)
    {
        s_Data.QuadPosition = position;
    }

    void Renderer2D::SetQuadRotation(const glm::vec3& rotation)
    {
        s_Data.QuadRotation = rotation;
    }

    void Renderer2D::SetQuadSize(const glm::vec2& size)
    {
        s_Data.QuadSize = size;
    }

    void Renderer2D::SetQuadTexture(const Ref<Texture>& texture)
    {
        s_Data.QuadTexture = texture;
    }

    void Renderer2D::SetQuadTextureCoords(const glm::vec4& textureCoords)
    {
        s_Data.QuadTextureCoords = textureCoords;
    }

    void Renderer2D::SetQuadTintColor(const glm::vec4& tintColor)
    {
        s_Data.QuadTintColor = tintColor;
    }

    void Renderer2D::ResetQuadState()
    {
        s_Data.QuadPosition = { 0.0f, 0.0f, 0.0f };
        s_Data.QuadRotation = { 0.0f, 0.0f, 0.0f };
        s_Data.QuadSize = { 1.0f, 1.0f };
        s_Data.QuadTexture = nullptr;
        s_Data.QuadTextureCoords = { 0.0f, 0.0f, 1.0f, 1.0f };
        s_Data.QuadTintColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    void Renderer2D::SetCirclePosition(const glm::vec3& position)
    {
        s_Data.CirclePosition = position;
    }

    void Renderer2D::SetCircleRotation(const glm::vec3& rotation)
    {
        s_Data.CircleRotation = rotation;
    }

    void Renderer2D::SetCircleRadius(const glm::vec2& radius)
    {
        s_Data.CircleRadius = radius;
    }

    void Renderer2D::SetCircleTexture(const Ref<Texture>& texture)
    {
        s_Data.CircleTexture = texture;
    }

    void Renderer2D::SetCircleTextureCoords(const glm::vec4& textureCoords)
    {
        s_Data.CircleTextureCoords = textureCoords;
    }

    void Renderer2D::SetCircleColor(const glm::vec4& color)
    {
        s_Data.CircleColor = color;
    }

    void Renderer2D::SetCircleThickness(float thickness)
    {
        s_Data.CircleThickness = thickness;
    }

    void Renderer2D::SetCircleFade(float fade)
    {
        s_Data.CircleFade = fade;
    }

    void Renderer2D::ResetCircleState()
    {
        s_Data.CirclePosition = { 0.0f, 0.0f, 0.0f };
        s_Data.CircleRotation = { 0.0f, 0.0f, 0.0f };
        s_Data.CircleRadius = { 1.0f, 1.0f };
        s_Data.CircleTexture = nullptr;
        s_Data.CircleTextureCoords = { 0.0f, 0.0f, 1.0f, 1.0f };
        s_Data.CircleColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        s_Data.CircleThickness = 1.0f;
        s_Data.CircleFade = 0.0f;
    }

    void Renderer2D::SetLineStart(const glm::vec3& start)
    {
        s_Data.LineStart = start;
    }

    void Renderer2D::SetLineEnd(const glm::vec3& end)
    {
        s_Data.LineEnd = end;
    }

    void Renderer2D::SetLineThickness(float thickness)
    {
        s_Data.LineThickness = thickness;
    }

    void Renderer2D::SetLineColor(const glm::vec4& color)
    {
        s_Data.LineColor = color;
    }

    void Renderer2D::ResetLineState()
    {
        s_Data.LineStart = { 0.0f, 0.0f, 0.0f };
        s_Data.LineEnd = { 1.0f, 1.0f, 0.0f };
        s_Data.LineThickness = 2.0f;
        s_Data.LineColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    void Renderer2D::SetStringContent(const std::string& text)
    {
        s_Data.TextContent = text;
    }

    void Renderer2D::SetStringPosition(const glm::vec3& position)
    {
        s_Data.TextPosition = position;
    }

    void Renderer2D::SetStringColor(const glm::vec4& color)
    {
        s_Data.TextColor = color;
    }

    void Renderer2D::SetStringSize(float size)
    {
        s_Data.TextSize = size;
    }

    void Renderer2D::SetStringFont(const Ref<Texture>& fontTexture)
    {
        s_Data.TextFont = fontTexture;
    }

    void Renderer2D::ResetStringState()
    {
        s_Data.TextContent = "";
        s_Data.TextPosition = { 0.0f, 0.0f, 0.0f };
        s_Data.TextColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        s_Data.TextSize = 1.0f;
        s_Data.TextFont = nullptr;
    }

    void Renderer2D::SetPixelPosition(const glm::vec3& position)
    {
        s_Data.PixelPosition = position;
    }

    void Renderer2D::SetPixelColor(const glm::vec4& color)
    {
        s_Data.PixelColor = color;
    }

    void Renderer2D::SetPixelSize(float size)
    {
        if (s_Data.PixelSize != size)
        {
            if (s_Data.PixelVertexCount > 0)
            {
                EndBatch();
                StartBatch();
            }

            s_Data.PixelSize = size;
        }
	}

    void Renderer2D::ResetPixelState()
    {
        s_Data.PixelPosition = { 0.0f, 0.0f, 0.0f };
        s_Data.PixelColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    void Renderer2D::SetTrianglePoint1(const glm::vec3& point1)
    {
        s_Data.TrianglePoint1 = point1;
    }

    void Renderer2D::SetTrianglePoint2(const glm::vec3& point2)
    {
        s_Data.TrianglePoint2 = point2;
    }

    void Renderer2D::SetTrianglePoint3(const glm::vec3& point3)
    {
        s_Data.TrianglePoint3 = point3;
    }

    void Renderer2D::SetTriangleTexture(const Ref<Texture>& texture)
    {
        s_Data.TriangleTexture = texture;
    }

    void Renderer2D::SetTriangleColor(const glm::vec4& color)
    {
        s_Data.TriangleColor = color;
    }

    void Renderer2D::ResetTriangleState()
    {
        s_Data.TrianglePoint1 = { 0.0f, 0.0f, 0.0f };
        s_Data.TrianglePoint2 = { 1.0f, 0.0f, 0.0f };
        s_Data.TrianglePoint3 = { 0.5f, 1.0f, 0.0f };
        s_Data.TriangleTexture = nullptr;
        s_Data.TriangleColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    }

    void Renderer2D::SetGridPosition(const glm::vec3& position)
    {
        s_Data.GridPosition = position;
    }

    void Renderer2D::SetGridRotation(const glm::vec3& rotation)
    {
        s_Data.GridRotation = rotation;
    }

    void Renderer2D::SetGridSize(const glm::vec2& size)
    {
        s_Data.GridSize = size;
    }

    void Renderer2D::SetGridCellSize(float gridSize)
    {
        s_Data.GridCellSize = gridSize;
    }

    void Renderer2D::SetGridColor(const glm::vec4& color)
    {
        s_Data.GridColor = color;
    }

    void Renderer2D::SetGridLineWidth(float lineWidth)
    {
        s_Data.GridLineWidth = lineWidth;
    }

    void Renderer2D::SetGridShowCheckerboard(bool showCheckerboard)
    {
        s_Data.GridShowCheckerboard = showCheckerboard;
    }

    void Renderer2D::SetGridCheckerColor1(const glm::vec4& checkerColor1)
    {
        s_Data.GridCheckerColor1 = checkerColor1;
    }

    void Renderer2D::SetGridCheckerColor2(const glm::vec4& checkerColor2)
    {
        s_Data.GridCheckerColor2 = checkerColor2;
    }

    void Renderer2D::ResetGridState()
    {
        s_Data.GridPosition = { 0.0f, 0.0f, 0.0f };
        s_Data.GridRotation = { 0.0f, 0.0f, 0.0f };
        s_Data.GridSize = { 1000.0f, 1000.0f };
        s_Data.GridCellSize = 1.0f;
        s_Data.GridColor = { 0.3f, 0.3f, 0.3f, 0.8f };
        s_Data.GridLineWidth = 1.0f;
        s_Data.GridShowCheckerboard = true;
        s_Data.GridCheckerColor1 = { 0.9f, 0.9f, 0.9f, 0.2f };
        s_Data.GridCheckerColor2 = { 0.8f, 0.8f, 0.8f, 0.2f };
    }

    void Renderer2D::SetPointLightPosition(const glm::vec3& position)
    {
        s_Data.PointLightPosition = position;
    }

    void Renderer2D::SetPointLightIntensity(float intensity)
    {
        s_Data.PointLightIntensity = intensity;
    }

    void Renderer2D::SetPointLightColor(const glm::vec4& color)
    {
        s_Data.PointLightColor = glm::vec3(color.r, color.g, color.b);
    }

    void Renderer2D::SetPointLightRadius(float radius)
    {
        s_Data.PointLightRadius = radius;
    }

    void Renderer2D::SetPointLightBlendMode(LightBlendMode blendMode)
    {
        s_Data.PointLightBlendMode = blendMode;
	}

    void Renderer2D::SetPointLightFalloff(float falloff)
    {
        s_Data.PointLightFalloff = falloff;
	}

    void Renderer2D::ResetPointLightState()
    {
        s_Data.PointLightPosition = { 0.0f, 0.0f, 0.0f };
        s_Data.PointLightIntensity = 1.0f;
        s_Data.PointLightColor = { 1.0f, 1.0f, 1.0f };
        s_Data.PointLightRadius = 5.0f;
        s_Data.PointLightBlendMode = LightBlendMode::Additive;
        s_Data.PointLightFalloff = 1.0f;
	}

    

    void Renderer2D::DrawQuad()
    {
        LUMINA_ASSERT(s_Data.QuadVertexBufferPtr >= s_Data.QuadVertexBufferBase, "Vertex buffer pointer underflow");

        if (s_Data.QuadIndexCount >= MaxIndices)
        {
            EndBatch();
            StartBatch();
        }

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), s_Data.QuadPosition);
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(s_Data.QuadRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(s_Data.QuadRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(s_Data.QuadRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 rotation = rotationZ * rotationY * rotationX;
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(s_Data.QuadSize, 1.0f));
        glm::mat4 transform = translation * rotation * scale;

        float texIndex = ComputeTextureIndex(s_Data.QuadTexture);

        glm::vec2 uvMin = { s_Data.QuadTextureCoords.x, s_Data.QuadTextureCoords.y };
        glm::vec2 uvMax = { s_Data.QuadTextureCoords.z, s_Data.QuadTextureCoords.w };

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
            s_Data.QuadVertexBufferPtr->Color = s_Data.QuadTintColor;
            s_Data.QuadVertexBufferPtr->TexCoord = uvs[i];
            s_Data.QuadVertexBufferPtr->TexIndex = texIndex;
            s_Data.QuadVertexBufferPtr++;
        }

        s_Data.QuadIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawCircle()
    {
        LUMINA_ASSERT(s_Data.CircleVertexBufferPtr >= s_Data.CircleVertexBufferBase, "Vertex buffer pointer underflow");

        if (s_Data.CircleIndexCount >= MaxIndices)
        {
            EndBatch();
            StartBatch();
        }

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), s_Data.CirclePosition);
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), s_Data.CircleRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), s_Data.CircleRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), s_Data.CircleRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 rotation = rotationZ * rotationY * rotationX;
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(s_Data.CircleRadius.x, s_Data.CircleRadius.y, 1.0f));
        glm::mat4 transform = translation * rotation * scale;

        float texIndex = ComputeTextureIndex(s_Data.CircleTexture);

        glm::vec2 uvMin = { s_Data.CircleTextureCoords.x, s_Data.CircleTextureCoords.y };
        glm::vec2 uvMax = { s_Data.CircleTextureCoords.z, s_Data.CircleTextureCoords.w };

        glm::vec2 uvs[4] =
        {
            { uvMin.x, uvMin.y },
            { uvMax.x, uvMin.y },
            { uvMax.x, uvMax.y },
            { uvMin.x, uvMax.y }
        };

        for (size_t i = 0; i < 4; i++)
        {
            s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
            s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.CircleVertexPositions[i];
            s_Data.CircleVertexBufferPtr->Color = s_Data.CircleColor;
            s_Data.CircleVertexBufferPtr->TexCoord = uvs[i];
            s_Data.CircleVertexBufferPtr->TexIndex = texIndex;
            s_Data.CircleVertexBufferPtr->Thickness = s_Data.CircleThickness;
            s_Data.CircleVertexBufferPtr->Fade = s_Data.CircleFade;
            s_Data.CircleVertexBufferPtr++;
        }

        s_Data.CircleIndexCount += 6;
        s_Data.Stats.CircleCount++;
    }

    void Renderer2D::DrawLine()
    {
        if (s_Data.LineVertexCount >= MaxVertices)
        {
            EndBatch();
            StartBatch();
        }

        if (s_Data.LineWidth != s_Data.LineThickness)
        {
            EndBatch();
            StartBatch();
            s_Data.LineWidth = s_Data.LineThickness;
        }

        s_Data.LineVertexBufferPtr->Position = s_Data.LineStart;
        s_Data.LineVertexBufferPtr->Color = s_Data.LineColor;
        s_Data.LineVertexBufferPtr++;
        s_Data.LineVertexCount++;

        s_Data.LineVertexBufferPtr->Position = s_Data.LineEnd;
        s_Data.LineVertexBufferPtr->Color = s_Data.LineColor;
        s_Data.LineVertexBufferPtr++;
        s_Data.LineVertexCount++;

        s_Data.Stats.LineCount++;
    }

    void Renderer2D::DrawString()
    {
        if (s_Data.TextContent.empty())
            return;

        if (s_Data.TextIndexCount >= MaxIndices)
        {
            EndBatch();
            StartBatch();
        }

        Ref<Texture> fontToUse = s_Data.TextFont ? s_Data.TextFont : s_Data.DefaultFont;
        float texIndex = ComputeTextureIndex(fontToUse);

        float xOffset = 0.0f;
        float charWidth = s_Data.TextSize;
        float charHeight = s_Data.TextSize;

        for (char c : s_Data.TextContent)
        {
            glm::vec3 charPos = s_Data.TextPosition + glm::vec3(xOffset, 0.0f, 0.0f);

            glm::mat4 translation = glm::translate(glm::mat4(1.0f), charPos);
            glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(charWidth, charHeight, 1.0f));
            glm::mat4 transform = translation * scale;

            float uvX = (c % 16) / 16.0f;
            float uvY = (c / 16) / 16.0f;
            float uvWidth = 1.0f / 16.0f;
            float uvHeight = 1.0f / 16.0f;

            glm::vec2 uvs[4] =
            {
                { uvX, uvY },
                { uvX + uvWidth, uvY },
                { uvX + uvWidth, uvY + uvHeight },
                { uvX, uvY + uvHeight }
            };

            for (size_t i = 0; i < 4; i++)
            {
                s_Data.TextVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
                s_Data.TextVertexBufferPtr->Color = s_Data.TextColor;
                s_Data.TextVertexBufferPtr->TexCoord = uvs[i];
                s_Data.TextVertexBufferPtr->TexIndex = texIndex;
                s_Data.TextVertexBufferPtr++;
            }

            s_Data.TextIndexCount += 6;
            xOffset += charWidth;
        }

        s_Data.Stats.TextCount++;
    }

    void Renderer2D::DrawPixel()
    {
        if (s_Data.PixelVertexCount >= MaxPixels)
        {
            EndBatch();
            StartBatch();
        }

        s_Data.PixelVertexBufferPtr->Position = s_Data.PixelPosition;
        s_Data.PixelVertexBufferPtr->Color = s_Data.PixelColor;
        s_Data.PixelVertexBufferPtr++;
        s_Data.PixelVertexCount++;

        s_Data.Stats.PixelCount++;
    }

    void Renderer2D::DrawTriangle()
    {
        if (s_Data.TriangleVertexCount >= MaxTriangles * 3)
        {
            EndBatch();
            StartBatch();
        }

        float texIndex = ComputeTextureIndex(s_Data.TriangleTexture);

        s_Data.TriangleVertexBufferPtr->Position = s_Data.TrianglePoint1;
        s_Data.TriangleVertexBufferPtr->Color = s_Data.TriangleColor;
        s_Data.TriangleVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
        s_Data.TriangleVertexBufferPtr->TexIndex = texIndex;
        s_Data.TriangleVertexBufferPtr++;

        s_Data.TriangleVertexBufferPtr->Position = s_Data.TrianglePoint2;
        s_Data.TriangleVertexBufferPtr->Color = s_Data.TriangleColor;
        s_Data.TriangleVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
        s_Data.TriangleVertexBufferPtr->TexIndex = texIndex;
        s_Data.TriangleVertexBufferPtr++;

        s_Data.TriangleVertexBufferPtr->Position = s_Data.TrianglePoint3;
        s_Data.TriangleVertexBufferPtr->Color = s_Data.TriangleColor;
        s_Data.TriangleVertexBufferPtr->TexCoord = { 0.5f, 1.0f };
        s_Data.TriangleVertexBufferPtr->TexIndex = texIndex;
        s_Data.TriangleVertexBufferPtr++;

        s_Data.TriangleVertexCount += 3;
        s_Data.Stats.TriangleCount++;
    }

    void Renderer2D::DrawGrid()
    {
        if (s_Data.GridIndexCount >= MaxIndices)
        {
            EndBatch();
            StartBatch();
        }

        glm::mat4 translation = glm::translate(glm::mat4(1.0f), s_Data.GridPosition);
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(s_Data.GridRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(s_Data.GridRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(s_Data.GridRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 rotation = rotationZ * rotationY * rotationX;
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(s_Data.GridSize, 1.0f));
        glm::mat4 transform = translation * rotation * scale;

        glm::vec2 texCoords[4] = {
            { 0.0f, 0.0f },
            { 1.0f, 0.0f },
            { 1.0f, 1.0f },
            { 0.0f, 1.0f }
        };

        for (size_t i = 0; i < 4; i++)
        {
            s_Data.GridVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.GridVertexBufferPtr->Color = s_Data.GridColor;
            s_Data.GridVertexBufferPtr->TexCoord = texCoords[i];
            s_Data.GridVertexBufferPtr->TexIndex = 0.0f;

            s_Data.GridVertexBufferPtr->GridPosition = s_Data.GridPosition;
            s_Data.GridVertexBufferPtr->GridSize = s_Data.GridSize;
            s_Data.GridVertexBufferPtr->CellSize = s_Data.GridCellSize;
            s_Data.GridVertexBufferPtr->GridColor = s_Data.GridColor;
            s_Data.GridVertexBufferPtr->LineWidth = s_Data.GridLineWidth;
            s_Data.GridVertexBufferPtr->ShowCheckerboard = s_Data.GridShowCheckerboard ? 1.0f : 0.0f;
            s_Data.GridVertexBufferPtr->CheckerColor1 = s_Data.GridCheckerColor1;
            s_Data.GridVertexBufferPtr->CheckerColor2 = s_Data.GridCheckerColor2;

            s_Data.GridVertexBufferPtr++;
        }

        s_Data.GridIndexCount += 6;
        s_Data.Stats.GridCount++;
    }

    void Renderer2D::DrawPointLight()
    {
        if (s_Data.PointLightCount >= MaxPointLights)
        {
            LUMINA_LOG_WARN("Maximum number of point lights ({}) exceeded", MaxPointLights);
            return;
        }

		s_Data.PointLightUniformBufferPtr->Position = s_Data.PointLightPosition;
		s_Data.PointLightUniformBufferPtr->Intensity = s_Data.PointLightIntensity;
		s_Data.PointLightUniformBufferPtr->Color = s_Data.PointLightColor;
		s_Data.PointLightUniformBufferPtr->Radius = s_Data.PointLightRadius;
        s_Data.PointLightUniformBufferPtr->BlendingMode = (int)s_Data.PointLightBlendMode;
		s_Data.PointLightUniformBufferPtr->Falloff = s_Data.PointLightFalloff;
        s_Data.PointLightUniformBufferPtr++;

        s_Data.PointLightCount++;
        s_Data.Stats.PointLightCount++;
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