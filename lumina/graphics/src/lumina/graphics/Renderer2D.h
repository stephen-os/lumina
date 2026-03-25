#pragma once

#include "Types.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "Sampler.h"
#include "Shader.h"
#include "InputLayout.h"
#include "BindingLayout.h"
#include "Pipeline.h"
#include "RenderTarget.h"
#include "FontAtlas.h"
#include "Camera2D.h"
#include "TextureAtlas.h"

#include <lumina/core/Base.h>

#include <glm/glm.hpp>

#include <array>
#include <map>
#include <stack>
#include <string>
#include <string_view>

namespace Lumina { class Device; }

namespace Lumina
{
    class Context;

    // ============================================================================
    // Enums
    // ============================================================================

    // Render layers control draw order. Lower values render first (background).
    // Users can use any uint32_t value for custom layers.
    enum class RenderLayer : uint32_t
    {
        Background = 0,
        World = 100,
        Sprites = 200,
        Effects = 300,
        UI = 400,
        Overlay = 500,

        // Default layer
        DefaultLayer = World
    };

    // Helper to create custom layer values
    constexpr RenderLayer MakeLayer(uint32_t value) { return static_cast<RenderLayer>(value); }

    enum class TextAlignment
    {
        Left,
        Center,
        Right
    };

    enum class LightBlendMode
    {
        Additive,
        Multiply,
        Screen,
        Overlay,
        SoftLight,
        LinearBurn,
        ColorDodge,
        Subtract,
        Alpha
    };

    enum class AttenuationModel
    {
        None,
        Linear,
        Quadratic,
        InverseSquare,
        Exponential,
        Smoothstep,
        Realistic
    };

    // ============================================================================
    // Descriptor Structs
    // ============================================================================

    struct QuadDesc
    {
        glm::vec3 Position = {0, 0, 0};
        glm::vec2 Size = {1, 1};
        glm::vec4 Color = {1, 1, 1, 1};
        float Rotation = 0.0f;
        glm::vec2 Origin = {0.5f, 0.5f};    // Rotation pivot (0-1 normalized)
        Ref<Texture> Texture = nullptr;
        glm::vec2 UVMin = {0, 0};
        glm::vec2 UVMax = {1, 1};
        RenderLayer Layer = RenderLayer::DefaultLayer;
        float Z = 0.0f;                     // Depth within layer (0.0-1.0)
        BlendMode Blend = BlendMode::Alpha;
    };

    struct CircleDesc
    {
        glm::vec3 Position = {0, 0, 0};
        glm::vec2 Radius = {1, 1};          // x,y for ellipse support
        glm::vec4 Color = {1, 1, 1, 1};
        float Thickness = 1.0f;             // 1.0 = filled, <1.0 = ring
        float Fade = 0.005f;                // Soft edge fade
        Ref<Texture> Texture = nullptr;
        glm::vec2 UVMin = {0, 0};
        glm::vec2 UVMax = {1, 1};
        RenderLayer Layer = RenderLayer::DefaultLayer;
        float Z = 0.0f;                     // Depth within layer (0.0-1.0)
        BlendMode Blend = BlendMode::Alpha;
    };

    struct LineDesc
    {
        glm::vec3 Start = {0, 0, 0};
        glm::vec3 End = {1, 0, 0};
        glm::vec4 Color = {1, 1, 1, 1};
        float Thickness = 1.0f;
        RenderLayer Layer = RenderLayer::DefaultLayer;
        float Z = 0.0f;                     // Depth within layer (0.0-1.0)
    };

    struct TextDesc
    {
        std::string_view Text;
        glm::vec3 Position = {0, 0, 0};
        float Scale = 1.0f;                 // Scale factor (1.0 = font's native pixel height)
        glm::vec4 Color = {1, 1, 1, 1};
        TextAlignment Alignment = TextAlignment::Left;
        Ref<FontAtlas> Font = nullptr;     // nullptr = default font
        RenderLayer Layer = RenderLayer::DefaultLayer;
        float Z = 0.0f;                     // Depth within layer (0.0-1.0)
    };

    struct TriangleDesc
    {
        glm::vec3 P0 = {0, 0, 0};
        glm::vec3 P1 = {1, 0, 0};
        glm::vec3 P2 = {0.5f, 1, 0};
        glm::vec4 Color = {1, 1, 1, 1};
        Ref<Texture> Texture = nullptr;
        glm::vec2 UV0 = {0, 1};
        glm::vec2 UV1 = {1, 1};
        glm::vec2 UV2 = {0.5f, 0};
        RenderLayer Layer = RenderLayer::DefaultLayer;
        float Z = 0.0f;                     // Depth within layer (0.0-1.0)
        BlendMode Blend = BlendMode::Alpha;
    };

    struct PixelDesc
    {
        glm::vec3 Position = {0, 0, 0};
        glm::vec4 Color = {1, 1, 1, 1};
        float Size = 1.0f;
        RenderLayer Layer = RenderLayer::DefaultLayer;
        float Z = 0.0f;                     // Depth within layer (0.0-1.0)
    };

    struct RectDesc
    {
        glm::vec3 Position = {0, 0, 0};
        glm::vec2 Size = {1, 1};
        glm::vec4 Color = {1, 1, 1, 1};
        float Thickness = 1.0f;
        float Rotation = 0.0f;
        RenderLayer Layer = RenderLayer::DefaultLayer;
        float Z = 0.0f;                     // Depth within layer (0.0-1.0)
    };

    struct GridDesc
    {
        glm::vec3 Position = {0, 0, 0};
        glm::vec2 Size = {10, 10};
        float CellSize = 1.0f;
        glm::vec4 LineColor = {0.5f, 0.5f, 0.5f, 1.0f};
        float LineWidth = 1.0f;
        bool Checkerboard = false;
        glm::vec4 CheckerColor1 = {0.4f, 0.4f, 0.4f, 1.0f};
        glm::vec4 CheckerColor2 = {0.6f, 0.6f, 0.6f, 1.0f};
        RenderLayer Layer = RenderLayer::DefaultLayer;
        float Z = 0.0f;                     // Depth within layer (0.0-1.0)
    };

    struct PointLightDesc
    {
        glm::vec3 Position = {0, 0, 0};
        glm::vec3 Color = {1, 1, 1};
        float Intensity = 1.0f;
        float Radius = 10.0f;
        LightBlendMode Blend = LightBlendMode::Additive;
        float BlendAlpha = 1.0f;
        AttenuationModel Attenuation = AttenuationModel::Quadratic;
        float Falloff = 1.0f;
        RenderLayer Layer = RenderLayer::Effects;
    };

    struct SpriteDesc
    {
        glm::vec3 Position = {0, 0, 0};
        glm::vec2 Size = {0, 0};            // If {0,0}, uses region size
        glm::vec4 Color = {1, 1, 1, 1};
        float Rotation = 0.0f;
        glm::vec2 Origin = {0.5f, 0.5f};    // Rotation pivot (0-1 normalized)
        bool FlipX = false;                 // Horizontal flip
        bool FlipY = false;                 // Vertical flip
        RenderLayer Layer = RenderLayer::DefaultLayer;
        float Z = 0.0f;                     // Depth within layer (0.0-1.0)
        BlendMode Blend = BlendMode::Alpha;
    };

    // ============================================================================
    // Statistics
    // ============================================================================

    struct Renderer2DStats
    {
        uint32_t DrawCalls = 0;
        uint32_t QuadCount = 0;
        uint32_t CircleCount = 0;
        uint32_t LineCount = 0;
        uint32_t TextCharCount = 0;
        uint32_t TriangleCount = 0;
        uint32_t PixelCount = 0;
        uint32_t GridCount = 0;
        uint32_t PointLightCount = 0;
        uint32_t TextureBinds = 0;

        [[nodiscard]] uint32_t GetTotalPrimitives() const noexcept
        {
            return QuadCount + CircleCount + LineCount + TriangleCount + PixelCount + GridCount;
        }

        void Reset() noexcept
        {
            DrawCalls = 0;
            QuadCount = 0;
            CircleCount = 0;
            LineCount = 0;
            TextCharCount = 0;
            TriangleCount = 0;
            PixelCount = 0;
            GridCount = 0;
            PointLightCount = 0;
            TextureBinds = 0;
        }
    };

    // ============================================================================
    // Configuration
    // ============================================================================

    struct Renderer2DConfig
    {
        uint32_t MaxQuads = 10000;
        uint32_t MaxCircles = 10000;
        uint32_t MaxLines = 10000;
        uint32_t MaxTriangles = 10000;
        uint32_t MaxTextChars = 10000;
        uint32_t MaxPixels = 50000;
        uint32_t MaxGrids = 1000;
        uint32_t MaxPointLights = 32;
        uint32_t MaxTextures = 32;
    };

    // ============================================================================
    // Renderer2D Class
    // ============================================================================

    class Renderer2D
    {
    public:
        explicit Renderer2D(Device& dev);
        ~Renderer2D();

        Renderer2D(const Renderer2D&) = delete;
        Renderer2D& operator=(const Renderer2D&) = delete;

        // Initialization
        bool Init(const Renderer2DConfig& config = {});
        void Shutdown();

        // Scene management
        void Begin(const glm::mat4& projection);
        void Begin(const glm::mat4& view, const glm::mat4& projection);
        void Begin(const Camera2D& camera);
        void End();

        // Render target
        void SetRenderTarget(Ref<RenderTarget> target);
        void SetDefaultRenderTarget();
        void Clear(const glm::vec4& color = {0.0f, 0.0f, 0.0f, 1.0f});

        // ========================================================================
        // Drawing API (struct-based)
        // ========================================================================

        void DrawQuad(const QuadDesc& desc);
        void DrawCircle(const CircleDesc& desc);
        void DrawLine(const LineDesc& desc);
        void DrawText(const TextDesc& desc);
        void SetDefaultFont(Ref<FontAtlas> font) { m_DefaultFont = font; }
        [[nodiscard]] Ref<FontAtlas> GetDefaultFont() const noexcept { return m_DefaultFont; }
        void DrawTriangle(const TriangleDesc& desc);
        void DrawPixel(const PixelDesc& desc);
        void DrawRect(const RectDesc& desc);
        void DrawGrid(const GridDesc& desc);

        // Sprite drawing (texture atlas support)
        void DrawSprite(const TextureAtlas& atlas, const std::string& regionName, const SpriteDesc& desc);
        void DrawSprite(const TextureAtlas& atlas, uint32_t regionIndex, const SpriteDesc& desc);
        void DrawSprite(const AtlasRegion& region, Ref<Texture> atlasTexture, const SpriteDesc& desc);

        // ========================================================================
        // Scissor/Clipping Regions
        // ========================================================================

        // Push a scissor rectangle onto the stack. All subsequent drawing will be clipped to this region.
        // If there's already a scissor active, the new scissor is intersected with the current one.
        // x, y = top-left corner in screen coordinates, w, h = dimensions
        void PushScissor(float x, float y, float width, float height);
        void PushScissor(const glm::vec4& rect);  // rect = {x, y, width, height}

        // Pop the current scissor rectangle, restoring the previous one (or disabling scissor if stack is empty)
        void PopScissor();

        /// Checks if scissor is currently active.
        [[nodiscard]] bool HasScissor() const noexcept { return !m_ScissorStack.empty(); }

        /// Gets the current scissor rect (returns zero rect if no scissor active).
        [[nodiscard]] glm::vec4 GetCurrentScissor() const;

        // ========================================================================
        // Texture Filtering
        // ========================================================================

        void SetFilterMode(FilterMode mode);
        [[nodiscard]] FilterMode GetFilterMode() const noexcept { return m_FilterMode; }

        // ========================================================================
        // Lighting
        // ========================================================================

        void SetLightingEnabled(bool enabled);
        [[nodiscard]] bool IsLightingEnabled() const noexcept { return m_LightingEnabled; }

        void SetAmbientLight(const glm::vec3& color, float intensity = 1.0f);
        [[nodiscard]] const glm::vec3& GetAmbientColor() const noexcept { return m_AmbientColor; }
        [[nodiscard]] float GetAmbientIntensity() const noexcept { return m_AmbientIntensity; }

        void DrawPointLight(const PointLightDesc& desc);

        // ========================================================================
        // Stats
        // ========================================================================

        [[nodiscard]] const Renderer2DStats& GetStats() const noexcept { return m_Stats; }
        void ResetStats() noexcept { m_Stats.Reset(); }

    private:
        Device& m_Device;
        Scope<Context> m_Context;
        Scope<PipelineCache> m_PipelineCache;
        std::vector<Ref<BindingSet>> m_FrameBindingSets;  // Keep binding sets alive during frame
        Renderer2DConfig m_Config;
        bool m_Initialized = false;

        // Camera
        glm::mat4 m_ViewProjection{1.0f};

        // Scissor stack
        std::stack<glm::vec4> m_ScissorStack;  // Stack of scissor rects {x, y, width, height}

        // ========================================================================
        // Vertex Types (internal)
        // ========================================================================

        struct QuadVertex
        {
            glm::vec4 Position;     // World position (xyz) + padding
            glm::vec4 Color;
            glm::vec2 TexCoord;
            float TexIndex;
            float ZIndex;          // Depth within layer (0.0-1.0)
        };

        struct CircleVertex
        {
            glm::vec4 WorldPosition;   // World position (xyz) + padding
            glm::vec4 LocalPosition;   // Local coords for SDF
            glm::vec4 Color;
            glm::vec2 TexCoord;
            float TexIndex;
            float Thickness;
            float Fade;
            float ZIndex;              // Depth within layer (0.0-1.0)
            // Total: 72 bytes - must match input layout stride
        };

        struct LineVertex
        {
            glm::vec4 Position;     // World position (xyz) + z_index in w
            glm::vec4 Color;
        };

        struct TextVertex
        {
            glm::vec4 Position;     // World position (xyz) + padding
            glm::vec4 Color;
            glm::vec2 TexCoord;
            float TexIndex;
            float ZIndex;          // Depth within layer (0.0-1.0)
        };

        struct TriangleVertex
        {
            glm::vec4 Position;     // World position (xyz) + padding
            glm::vec4 Color;
            glm::vec2 TexCoord;
            float TexIndex;
            float ZIndex;          // Depth within layer (0.0-1.0)
        };

        struct PixelVertex
        {
            glm::vec4 Position;     // World position (xyz) + z_index in w
            glm::vec4 Color;
            float Size;
            float _pad[3];
        };

        struct GridVertex
        {
            glm::vec4 Position;         // World position (xyz) + z_index in w
            glm::vec4 LocalPosition;   // For procedural grid in shader
            glm::vec4 LineColor;
            glm::vec2 GridSize;
            float CellSize;
            float LineWidth;
            float ShowCheckerboard;
            glm::vec4 CheckerColor1;
            glm::vec4 CheckerColor2;
        };

        // ========================================================================
        // Layer Batch (per-layer vertex storage)
        // ========================================================================

        struct LayerBatch
        {
            std::vector<QuadVertex> QuadVertices;
            std::vector<CircleVertex> CircleVertices;
            std::vector<LineVertex> LineVertices;
            std::vector<TextVertex> TextVertices;
            std::vector<TriangleVertex> TriangleVertices;
            std::vector<PixelVertex> PixelVertices;
            std::vector<GridVertex> GridVertices;

            uint32_t QuadCount = 0;
            uint32_t CircleCount = 0;
            uint32_t LineCount = 0;
            uint32_t TextCharCount = 0;
            uint32_t TriangleCount = 0;
            uint32_t PixelCount = 0;
            uint32_t GridCount = 0;

            // Texture batching per layer
            std::array<Ref<Texture>, 32> TextureSlots;
            uint32_t TextureSlotIndex = 0;

            // Blend mode tracking per primitive type
            BlendMode QuadBlend = BlendMode::Alpha;
            BlendMode CircleBlend = BlendMode::Alpha;
            BlendMode TriangleBlend = BlendMode::Alpha;

            void Clear()
            {
                QuadVertices.clear();
                CircleVertices.clear();
                LineVertices.clear();
                TextVertices.clear();
                TriangleVertices.clear();
                PixelVertices.clear();
                GridVertices.clear();

                QuadCount = 0;
                CircleCount = 0;
                LineCount = 0;
                TextCharCount = 0;
                TriangleCount = 0;
                PixelCount = 0;
                GridCount = 0;

                TextureSlots.fill(nullptr);
                TextureSlotIndex = 0;

                QuadBlend = BlendMode::Alpha;
                CircleBlend = BlendMode::Alpha;
                TriangleBlend = BlendMode::Alpha;
            }

            bool IsEmpty() const
            {
                return QuadCount == 0 && CircleCount == 0 && LineCount == 0 &&
                       TextCharCount == 0 && TriangleCount == 0 && PixelCount == 0 &&
                       GridCount == 0;
            }
        };

        // ========================================================================
        // Batch Limits
        // ========================================================================

        static constexpr uint32_t VerticesPerQuad = 4;
        static constexpr uint32_t IndicesPerQuad = 6;

        // ========================================================================
        // Per-Layer Batches (sorted map for ordered iteration)
        // ========================================================================

        std::map<uint32_t, LayerBatch> m_Layers;

        // ========================================================================
        // Private Methods
        // ========================================================================

        void FlushLayer(uint32_t layerId);
        void FlushLayersUpTo(uint32_t layerId);  // Flush all layers below this layer
        void FlushQuads(uint32_t layerId);
        void FlushCircles(uint32_t layerId);
        void FlushLines(uint32_t layerId);
        void FlushText(uint32_t layerId);
        void FlushTriangles(uint32_t layerId);
        void FlushPixels(uint32_t layerId);
        void FlushGrids(uint32_t layerId);
        void FlushAll();

        void StartBatch();
        float GetTextureIndex(uint32_t layerId, Ref<Texture> tex);
        LayerBatch& GetLayer(uint32_t layerId);
        void ApplyScissor();  // Apply current scissor state to context

        // ========================================================================
        // Shared GPU Resources (pipelines, shaders, buffers - shared across layers)
        // ========================================================================

        // Quad
        Ref<VertexBuffer> m_QuadVertexBuffer;
        Ref<IndexBuffer> m_QuadIndexBuffer;
        Ref<Shader> m_QuadShader;
        Ref<InputLayout> m_QuadInputLayout;
        Ref<BindingLayout> m_QuadBindingLayout;
        Ref<Pipeline> m_QuadPipeline;
        uint32_t m_QuadVertexOffset = 0;  // Current vertex offset for appending (in vertices, not bytes)

        // Circle
        Ref<VertexBuffer> m_CircleVertexBuffer;
        Ref<IndexBuffer> m_CircleIndexBuffer;
        Ref<Shader> m_CircleShader;
        Ref<InputLayout> m_CircleInputLayout;
        Ref<BindingLayout> m_CircleBindingLayout;
        Ref<Pipeline> m_CirclePipeline;
        uint32_t m_CircleVertexOffset = 0;

        // Line
        Ref<VertexBuffer> m_LineVertexBuffer;
        Ref<Shader> m_LineShader;
        Ref<InputLayout> m_LineInputLayout;
        Ref<BindingLayout> m_LineBindingLayout;
        Ref<Pipeline> m_LinePipeline;
        uint32_t m_LineVertexOffset = 0;

        // Text
        Ref<VertexBuffer> m_TextVertexBuffer;
        Ref<IndexBuffer> m_TextIndexBuffer;
        Ref<Shader> m_TextShader;
        Ref<InputLayout> m_TextInputLayout;
        Ref<BindingLayout> m_TextBindingLayout;
        Ref<Pipeline> m_TextPipeline;
        Ref<FontAtlas> m_DefaultFont;
        uint32_t m_TextVertexOffset = 0;

        // Triangle
        Ref<VertexBuffer> m_TriangleVertexBuffer;
        Ref<Shader> m_TriangleShader;
        Ref<InputLayout> m_TriangleInputLayout;
        Ref<BindingLayout> m_TriangleBindingLayout;
        Ref<Pipeline> m_TrianglePipeline;
        uint32_t m_TriangleVertexOffset = 0;

        // Pixel
        Ref<VertexBuffer> m_PixelVertexBuffer;
        Ref<Shader> m_PixelShader;
        Ref<InputLayout> m_PixelInputLayout;
        Ref<BindingLayout> m_PixelBindingLayout;
        Ref<Pipeline> m_PixelPipeline;
        uint32_t m_PixelVertexOffset = 0;

        // Grid
        Ref<VertexBuffer> m_GridVertexBuffer;
        Ref<IndexBuffer> m_GridIndexBuffer;
        Ref<Shader> m_GridShader;
        Ref<InputLayout> m_GridInputLayout;
        Ref<BindingLayout> m_GridBindingLayout;
        Ref<Pipeline> m_GridPipeline;
        uint32_t m_GridVertexOffset = 0;

        // ========================================================================
        // Shared Resources
        // ========================================================================

        Ref<Sampler> m_DefaultSampler;
        Ref<Sampler> m_PointSampler;
        Ref<Sampler> m_CurrentSampler;  // Active sampler (linear or point)
        FilterMode m_FilterMode = FilterMode::Linear;
        Ref<Texture> m_WhiteTexture;

        // Camera constant buffer (shared by all primitives)
        struct CameraData
        {
            glm::mat4 ViewProjection;
        };
        Ref<UniformBuffer> m_CameraBuffer;

        // ========================================================================
        // Lighting State
        // ========================================================================

        bool m_LightingEnabled = false;
        glm::vec3 m_AmbientColor{0.1f, 0.1f, 0.1f};
        float m_AmbientIntensity = 1.0f;

        struct PointLightData
        {
            glm::vec3 Position;
            float Intensity;
            glm::vec3 Color;
            float Radius;
            float BlendMode;
            float BlendAlpha;
            float Attenuation;
            float Falloff;
        };

        std::vector<PointLightData> m_PointLights;

        // ========================================================================
        // Lighting Resources (Godot-style deferred 2D lighting)
        // ========================================================================

        // Render targets for lighting pass
        Ref<RenderTarget> m_SceneTarget;          // Scene is rendered here when lighting enabled
        Ref<RenderTarget> m_LightAccumulation;    // Light contributions accumulated here

        // Fullscreen quad for lighting passes
        struct FullscreenVertex
        {
            glm::vec4 Position;
            glm::vec2 TexCoord;
        };
        Ref<VertexBuffer> m_FullscreenVertexBuffer;

        // Point light shader and pipeline
        Ref<Shader> m_PointLightShader;
        Ref<InputLayout> m_PointLightInputLayout;
        Ref<BindingLayout> m_PointLightBindingLayout;
        Ref<Pipeline> m_PointLightPipeline;

        // Composite shader and pipeline
        Ref<Shader> m_CompositeShader;
        Ref<InputLayout> m_CompositeInputLayout;
        Ref<BindingLayout> m_CompositeBindingLayout;
        Ref<Pipeline> m_CompositePipeline;

        // Constant buffers for lighting passes
        Ref<UniformBuffer> m_LightParamsBuffer;
        Ref<UniformBuffer> m_CompositeParamsBuffer;

        // Cached viewport size for lighting target resize
        uint32_t m_LightingTargetWidth = 0;
        uint32_t m_LightingTargetHeight = 0;

        // Private lighting methods
        void InitLightingResources();
        void ShutdownLightingResources();
        void EnsureLightingTargets(uint32_t width, uint32_t height);
        void FlushLights();
        void CompositeScene();

        // ========================================================================
        // Render Target State
        // ========================================================================

        Ref<RenderTarget> m_CurrentTarget;
        Format m_CurrentColorFormat = Format::RGBA8Unorm;
        Format m_CurrentDepthFormat = Format::Unknown;
        uint32_t m_CurrentSampleCount = 1;  // MSAA sample count

        // Stats
        Renderer2DStats m_Stats;
    };
}
