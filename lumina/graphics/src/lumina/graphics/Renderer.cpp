#include "Renderer.h"
#include <lumina/core/Application.h>
#include <lumina/core/Log.h>

#include <glm/gtc/matrix_transform.hpp>

#include <unordered_map>

namespace Lumina
{
    // ============================================================================
    // Internal Validation Macros
    // ============================================================================

    // Check that renderer is initialized, return early if not
    #define RENDERER_CHECK_INIT() \
        do { \
            if (!s_State) { \
                LUMINA_LOG_ERROR("Renderer not initialized"); \
                return; \
            } \
        } while (0)

    // Check that renderer is initialized, return value if not
    #define RENDERER_CHECK_INIT_RET(ret) \
        do { \
            if (!s_State) { \
                LUMINA_LOG_ERROR("Renderer not initialized"); \
                return ret; \
            } \
        } while (0)

    // ============================================================================
    // Static State
    // ============================================================================

    struct RendererState
    {
        Scope<Renderer2D> Renderer;

        // Context management
        struct ContextData
        {
            Ref<RenderTarget> Target;
            uint32_t Width = 0;
            uint32_t Height = 0;
            uint32_t SampleCount = 1;
        };

        std::unordered_map<uint32_t, ContextData> Contexts;
        uint32_t NextContextId = 1;  // 0 is reserved for default

        // Default context
        ContextData DefaultContext;

        // Current state
        uint32_t CurrentContextId = 0;
        bool InFrame = false;

        // Default projection (orthographic, top-left origin)
        glm::mat4 CurrentProjection{1.0f};
        bool UseCustomProjection = false;
    };

    static RendererState* s_State = nullptr;

    // ============================================================================
    // Helper Functions
    // ============================================================================

    static glm::mat4 MakeDefaultProjection(uint32_t width, uint32_t height)
    {
        return glm::ortho(0.0f, static_cast<float>(width),
                          static_cast<float>(height), 0.0f,
                          -1.0f, 1.0f);
    }

    static RendererState::ContextData* GetContextData(uint32_t id)
    {
        if (id == 0)
        {
            return &s_State->DefaultContext;
        }

        auto it = s_State->Contexts.find(id);
        if (it != s_State->Contexts.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    // ============================================================================
    // Initialization
    // ============================================================================

    void Renderer::Init(RendererConfig config)
    {
        if (s_State != nullptr)
        {
            LUMINA_LOG_WARN("Renderer already initialized");
            return;
        }

        s_State = new RendererState();

        auto& device = Core::Application::Get().GetDevice();

        // Create Renderer2D
        s_State->Renderer = MakeScope<Renderer2D>(device);
        s_State->Renderer->Init();

        // Create default context with MSAA
        s_State->DefaultContext.Target = RenderTarget::Create(
            device, config.Width, config.Height, Format::RGBA8Unorm, Format::Unknown, static_cast<uint32_t>(config.Msaa)
        );
        s_State->DefaultContext.Width = config.Width;
        s_State->DefaultContext.Height = config.Height;
        s_State->DefaultContext.SampleCount = static_cast<uint32_t>(config.Msaa);

        // Set default projection
        s_State->CurrentProjection = MakeDefaultProjection(config.Width, config.Height);
    }

    void Renderer::Shutdown()
    {
        RENDERER_CHECK_INIT();

        // Cleanup contexts
        s_State->Contexts.clear();
        s_State->DefaultContext.Target.reset();

        // Shutdown renderer
        s_State->Renderer->Shutdown();
        s_State->Renderer.reset();

        delete s_State;
        s_State = nullptr;
    }

    bool Renderer::IsInitialized()
    {
        return s_State != nullptr;
    }

    // ============================================================================
    // Context Management
    // ============================================================================

    RenderContext Renderer::CreateContext(uint32_t width, uint32_t height)
    {
        return CreateContext(width, height, MsaaMode::None);
    }

    RenderContext Renderer::CreateContext(uint32_t width, uint32_t height, MsaaMode msaa)
    {
        RENDERER_CHECK_INIT_RET(RenderContext{0});

        auto& device = Core::Application::Get().GetDevice();

        RendererState::ContextData ctxData;
        ctxData.Target = RenderTarget::Create(device, width, height, Format::RGBA8Unorm, Format::Unknown, static_cast<uint32_t>(msaa));
        ctxData.Width = width;
        ctxData.Height = height;
        ctxData.SampleCount = static_cast<uint32_t>(msaa);

        uint32_t id = s_State->NextContextId++;
        s_State->Contexts[id] = std::move(ctxData);

        return RenderContext{id};
    }

    void Renderer::DestroyContext(RenderContext ctx)
    {
        RENDERER_CHECK_INIT();

        if (ctx.Id == 0)
        {
            LUMINA_LOG_WARN("Cannot destroy default context");
            return;
        }

        s_State->Contexts.erase(ctx.Id);
    }

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        RENDERER_CHECK_INIT();

        if (width == 0 || height == 0) return;
        if (s_State->DefaultContext.Width == width &&
            s_State->DefaultContext.Height == height) return;

        auto& device = Core::Application::Get().GetDevice();

        // Preserve MSAA sample count when resizing
        uint32_t sampleCount = s_State->DefaultContext.SampleCount;
        s_State->DefaultContext.Target = RenderTarget::Create(
            device, width, height, Format::RGBA8Unorm, Format::Unknown, sampleCount
        );
        s_State->DefaultContext.Width = width;
        s_State->DefaultContext.Height = height;
    }

    void Renderer::Resize(RenderContext ctx, uint32_t width, uint32_t height)
    {
        RENDERER_CHECK_INIT();

        if (ctx.Id == 0)
        {
            Resize(width, height);
            return;
        }

        if (width == 0 || height == 0) return;

        auto* ctxData = GetContextData(ctx.Id);
        if (!ctxData) return;

        if (ctxData->Width == width && ctxData->Height == height) return;

        auto& device = Core::Application::Get().GetDevice();

        // Preserve MSAA sample count when resizing
        ctxData->Target = RenderTarget::Create(device, width, height, Format::RGBA8Unorm, Format::Unknown, ctxData->SampleCount);
        ctxData->Width = width;
        ctxData->Height = height;
    }

    glm::uvec2 Renderer::GetSize()
    {
        RENDERER_CHECK_INIT_RET(glm::uvec2(0, 0));
        return {s_State->DefaultContext.Width, s_State->DefaultContext.Height};
    }

    glm::uvec2 Renderer::GetSize(RenderContext ctx)
    {
        RENDERER_CHECK_INIT_RET(glm::uvec2(0, 0));

        auto* ctxData = GetContextData(ctx.Id);
        if (!ctxData) return {0, 0};

        return {ctxData->Width, ctxData->Height};
    }

    // ============================================================================
    // Rendering
    // ============================================================================

    void Renderer::Begin()
    {
        Begin(RenderContext{0});
    }

    void Renderer::Begin(RenderContext ctx)
    {
        RENDERER_CHECK_INIT();

        if (s_State->InFrame)
        {
            LUMINA_LOG_ERROR("Already in a frame, call End() first");
            return;
        }

        auto* ctxData = GetContextData(ctx.Id);
        if (!ctxData)
        {
            LUMINA_LOG_ERROR("Invalid render context");
            return;
        }

        s_State->CurrentContextId = ctx.Id;
        s_State->InFrame = true;

        // Use custom projection or generate default for this context
        glm::mat4 projection;
        if (s_State->UseCustomProjection)
        {
            projection = s_State->CurrentProjection;
        }
        else
        {
            projection = MakeDefaultProjection(ctxData->Width, ctxData->Height);
        }

        s_State->Renderer->Begin(projection);
        s_State->Renderer->SetRenderTarget(ctxData->Target);
    }

    void Renderer::End()
    {
        RENDERER_CHECK_INIT();

        if (!s_State->InFrame)
        {
            LUMINA_LOG_ERROR("Not in a frame, call Begin() first");
            return;
        }

        s_State->Renderer->End();
        s_State->InFrame = false;

        // Reset custom projection flag for next frame
        s_State->UseCustomProjection = false;
    }

    void Renderer::Clear(const glm::vec4& color)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->Clear(color);
    }

    // ============================================================================
    // Camera / Projection
    // ============================================================================

    void Renderer::SetCamera(const Camera2D& camera)
    {
        RENDERER_CHECK_INIT();
        s_State->CurrentProjection = camera.GetViewProjectionMatrix();
        s_State->UseCustomProjection = true;
    }

    void Renderer::SetProjection(const glm::mat4& projection)
    {
        RENDERER_CHECK_INIT();
        s_State->CurrentProjection = projection;
        s_State->UseCustomProjection = true;
    }

    // ============================================================================
    // Output
    // ============================================================================

    Ref<Texture> Renderer::GetTexture()
    {
        RENDERER_CHECK_INIT_RET(nullptr);

        auto* ctxData = GetContextData(0);
        if (!ctxData || !ctxData->Target) return nullptr;

        return ctxData->Target->GetColorTexture();
    }

    Ref<Texture> Renderer::GetTexture(RenderContext ctx)
    {
        RENDERER_CHECK_INIT_RET(nullptr);

        auto* ctxData = GetContextData(ctx.Id);
        if (!ctxData || !ctxData->Target) return nullptr;

        return ctxData->Target->GetColorTexture();
    }

    // ============================================================================
    // Drawing - Primitives
    // ============================================================================

    void Renderer::DrawQuad(const QuadDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawQuad(desc);
    }

    void Renderer::DrawCircle(const CircleDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawCircle(desc);
    }

    void Renderer::DrawLine(const LineDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawLine(desc);
    }

    void Renderer::DrawTriangle(const TriangleDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawTriangle(desc);
    }

    void Renderer::DrawRect(const RectDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawRect(desc);
    }

    void Renderer::DrawPixel(const PixelDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawPixel(desc);
    }

    void Renderer::DrawGrid(const GridDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawGrid(desc);
    }

    // ============================================================================
    // Drawing - Text
    // ============================================================================

    void Renderer::DrawText(const TextDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawText(desc);
    }

    void Renderer::SetDefaultFont(Ref<FontAtlas> font)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->SetDefaultFont(font);
    }

    Ref<FontAtlas> Renderer::GetDefaultFont()
    {
        RENDERER_CHECK_INIT_RET(nullptr);
        return s_State->Renderer->GetDefaultFont();
    }

    // ============================================================================
    // Drawing - Sprites
    // ============================================================================

    void Renderer::DrawSprite(const TextureAtlas& atlas, const std::string& regionName, const SpriteDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawSprite(atlas, regionName, desc);
    }

    void Renderer::DrawSprite(const TextureAtlas& atlas, uint32_t regionIndex, const SpriteDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawSprite(atlas, regionIndex, desc);
    }

    void Renderer::DrawSprite(const AtlasRegion& region, Ref<Texture> atlasTexture, const SpriteDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawSprite(region, atlasTexture, desc);
    }

    // ============================================================================
    // Scissor / Clipping
    // ============================================================================

    void Renderer::PushScissor(float x, float y, float width, float height)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->PushScissor(x, y, width, height);
    }

    void Renderer::PushScissor(const glm::vec4& rect)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->PushScissor(rect);
    }

    void Renderer::PopScissor()
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->PopScissor();
    }

    bool Renderer::HasScissor()
    {
        RENDERER_CHECK_INIT_RET(false);
        return s_State->Renderer->HasScissor();
    }

    // ============================================================================
    // Texture Filtering
    // ============================================================================

    void Renderer::SetFilterMode(FilterMode mode)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->SetFilterMode(mode);
    }

    FilterMode Renderer::GetFilterMode()
    {
        RENDERER_CHECK_INIT_RET(FilterMode::Linear);
        return s_State->Renderer->GetFilterMode();
    }

    // ============================================================================
    // Lighting
    // ============================================================================

    void Renderer::SetLightingEnabled(bool enabled)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->SetLightingEnabled(enabled);
    }

    bool Renderer::IsLightingEnabled()
    {
        RENDERER_CHECK_INIT_RET(false);
        return s_State->Renderer->IsLightingEnabled();
    }

    void Renderer::SetAmbientLight(const glm::vec3& color, float intensity)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->SetAmbientLight(color, intensity);
    }

    void Renderer::DrawPointLight(const PointLightDesc& desc)
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->DrawPointLight(desc);
    }

    // ============================================================================
    // Stats
    // ============================================================================

    const Renderer2DStats& Renderer::GetStats()
    {
        static const Renderer2DStats emptyStats{};
        if (!s_State)
        {
            LUMINA_LOG_ERROR("Renderer not initialized");
            return emptyStats;
        }
        return s_State->Renderer->GetStats();
    }

    void Renderer::ResetStats()
    {
        RENDERER_CHECK_INIT();
        s_State->Renderer->ResetStats();
    }
}
