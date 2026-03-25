#pragma once

#include "renderer2d.h"
#include "render_target.h"
#include "camera2d.h"
#include "texture.h"
#include "texture_atlas.h"
#include "font_atlas.h"

#include <lumina/core/base.h>

#include <glm/glm.hpp>

#include <cstdint>

namespace Lumina
{
    /// Opaque handle for render contexts
    struct RenderContext
    {
        uint32_t Id = 0;

        bool operator==(const RenderContext& other) const { return Id == other.Id; }
        bool operator!=(const RenderContext& other) const { return Id != other.Id; }
    };

    enum class MSAAMode
    {
        None = 1,   /// No multisampling
        X2   = 2,   /// 2 samples per pixel
        X4   = 4,   /// 4 samples per pixel
        X8   = 8,   /// 8 samples per pixel
        X16  = 16   /// 16 samples per pixel
    };

    struct RendererConfig
    {
        uint32_t Width = 800;              /// Initial width of the default render context
        uint32_t Height = 600;             /// Initial height of the default render context
        MSAAMode Msaa = MSAAMode::None;    /// MSAA sample count for the default context
    };

    /// Static 2D renderer with simplified API
    /// Wraps renderer2d and manages render contexts internally
    class Renderer
    {
    public:
        // ========================================================================
        // Initialization
        // ========================================================================

        /// Initialize with default configuration
        static void Init(RendererConfig config = {});

        /// Shutdown and cleanup all resources
        static void Shutdown();

        /// Check if renderer is initialized
        [[nodiscard]] static bool IsInitialized();

        // ========================================================================
        // Context Management
        // ========================================================================

        /// Create an additional render context
        [[nodiscard]] static RenderContext CreateContext(uint32_t width, uint32_t height);

        /// Create an additional render context with MSAA
        [[nodiscard]] static RenderContext CreateContext(uint32_t width, uint32_t height, MSAAMode msaa);

        /// Destroy a render context
        static void DestroyContext(RenderContext ctx);

        /// Resize the default context
        static void Resize(uint32_t width, uint32_t height);

        /// Resize a specific context
        static void Resize(RenderContext ctx, uint32_t width, uint32_t height);

        /// Get default context size
        [[nodiscard]] static glm::uvec2 GetSize();

        /// Get specific context size
        [[nodiscard]] static glm::uvec2 GetSize(RenderContext ctx);

        // ========================================================================
        // Rendering
        // ========================================================================

        /// Begin rendering to default context
        static void Begin();

        /// Begin rendering to specific context
        static void Begin(RenderContext ctx);

        /// End rendering and flush batches
        static void End();

        /// Clear current context with color
        static void Clear(const glm::vec4& color);

        // ========================================================================
        // Camera / Projection
        // ========================================================================

        /// Set camera for current context
        static void SetCamera(const Camera2D& camera);

        /// Set raw projection matrix for current context
        static void SetProjection(const glm::mat4& projection);

        // ========================================================================
        // Output
        // ========================================================================

        /// Get rendered texture from default context
        [[nodiscard]] static Ref<Texture> GetTexture();

        /// Get rendered texture from specific context
        [[nodiscard]] static Ref<Texture> GetTexture(RenderContext ctx);

        // ========================================================================
        // Drawing - Primitives
        // ========================================================================

        static void DrawQuad(const QuadDesc& desc);
        static void DrawCircle(const CircleDesc& desc);
        static void DrawLine(const LineDesc& desc);
        static void DrawTriangle(const TriangleDesc& desc);
        static void DrawRect(const RectDesc& desc);
        static void DrawPixel(const PixelDesc& desc);
        static void DrawGrid(const GridDesc& desc);

        // ========================================================================
        // Drawing - Text
        // ========================================================================

        static void DrawText(const TextDesc& desc);
        static void SetDefaultFont(Ref<FontAtlas> font);
        [[nodiscard]] static Ref<FontAtlas> GetDefaultFont();

        // ========================================================================
        // Drawing - Sprites
        // ========================================================================

        static void DrawSprite(const TextureAtlas& atlas, const std::string& regionName, const SpriteDesc& desc);
        static void DrawSprite(const TextureAtlas& atlas, uint32_t regionIndex, const SpriteDesc& desc);
        static void DrawSprite(const AtlasRegion& region, Ref<Texture> atlasTexture, const SpriteDesc& desc);

        // ========================================================================
        // Scissor / Clipping
        // ========================================================================

        static void PushScissor(float x, float y, float width, float height);
        static void PushScissor(const glm::vec4& rect);
        static void PopScissor();
        [[nodiscard]] static bool HasScissor();

        // ========================================================================
        // Texture Filtering
        // ========================================================================

        static void SetFilterMode(FilterMode mode);
        [[nodiscard]] static FilterMode GetFilterMode();

        // ========================================================================
        // Lighting
        // ========================================================================

        static void SetLightingEnabled(bool enabled);
        [[nodiscard]] static bool IsLightingEnabled();
        static void SetAmbientLight(const glm::vec3& color, float intensity = 1.0f);
        static void DrawPointLight(const PointLightDesc& desc);

        // ========================================================================
        // Stats
        // ========================================================================

        [[nodiscard]] static const Renderer2DStats& GetStats();
        static void ResetStats();

    private:
        Renderer() = delete;
        ~Renderer() = delete;
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
    };
}
