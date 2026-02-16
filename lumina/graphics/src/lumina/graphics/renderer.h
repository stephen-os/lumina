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

namespace lumina::graphics
{
    /// Opaque handle for render contexts
    struct render_context
    {
        uint32_t id = 0;

        bool operator==(const render_context& other) const { return id == other.id; }
        bool operator!=(const render_context& other) const { return id != other.id; }
    };

    /// Static 2D renderer with simplified API
    /// Wraps renderer2d and manages render contexts internally
    class renderer
    {
    public:
        // ========================================================================
        // Initialization
        // ========================================================================

        /// Initialize with default size (800x600)
        static void init();

        /// Initialize with explicit size
        static void init(uint32_t width, uint32_t height);

		/// Initialize with glm::uvec2 size
		static void init(const glm::uvec2& size);

        /// Shutdown and cleanup all resources
        static void shutdown();

        /// Check if renderer is initialized
        [[nodiscard]] static bool is_initialized();

        // ========================================================================
        // Context Management
        // ========================================================================

        /// Create an additional render context
        [[nodiscard]] static render_context create_context(uint32_t width, uint32_t height);

        /// Destroy a render context
        static void destroy_context(render_context ctx);

        /// Resize the default context
        static void resize(uint32_t width, uint32_t height);

        /// Resize a specific context
        static void resize(render_context ctx, uint32_t width, uint32_t height);

        /// Get default context size
        [[nodiscard]] static glm::uvec2 get_size();

        /// Get specific context size
        [[nodiscard]] static glm::uvec2 get_size(render_context ctx);

        // ========================================================================
        // Rendering
        // ========================================================================

        /// Begin rendering to default context
        static void begin();

        /// Begin rendering to specific context
        static void begin(render_context ctx);

        /// End rendering and flush batches
        static void end();

        /// Clear current context with color
        static void clear(const glm::vec4& color);

        // ========================================================================
        // Camera / Projection
        // ========================================================================

        /// Set camera for current context
        static void set_camera(const camera2d& camera);

        /// Set raw projection matrix for current context
        static void set_projection(const glm::mat4& projection);

        // ========================================================================
        // Output
        // ========================================================================

        /// Get rendered texture from default context
        [[nodiscard]] static ref<texture> get_texture();

        /// Get rendered texture from specific context
        [[nodiscard]] static ref<texture> get_texture(render_context ctx);

        // ========================================================================
        // Drawing - Primitives
        // ========================================================================

        static void draw_quad(const quad_desc& desc);
        static void draw_circle(const circle_desc& desc);
        static void draw_line(const line_desc& desc);
        static void draw_triangle(const triangle_desc& desc);
        static void draw_rect(const rect_desc& desc);
        static void draw_pixel(const pixel_desc& desc);
        static void draw_grid(const grid_desc& desc);

        // ========================================================================
        // Drawing - Text
        // ========================================================================

        static void draw_text(const text_desc& desc);
        static void set_default_font(ref<font_atlas> font);
        [[nodiscard]] static ref<font_atlas> get_default_font();

        // ========================================================================
        // Drawing - Sprites
        // ========================================================================

        static void draw_sprite(const texture_atlas& atlas, const std::string& region_name, const sprite_desc& desc);
        static void draw_sprite(const texture_atlas& atlas, uint32_t region_index, const sprite_desc& desc);
        static void draw_sprite(const atlas_region& region, ref<texture> atlas_texture, const sprite_desc& desc);

        // ========================================================================
        // Scissor / Clipping
        // ========================================================================

        static void push_scissor(float x, float y, float width, float height);
        static void push_scissor(const glm::vec4& rect);
        static void pop_scissor();
        [[nodiscard]] static bool has_scissor();

        // ========================================================================
        // Texture Filtering
        // ========================================================================

        static void set_filter_mode(filter_mode mode);
        [[nodiscard]] static filter_mode get_filter_mode();

        // ========================================================================
        // Lighting
        // ========================================================================

        static void set_lighting_enabled(bool enabled);
        [[nodiscard]] static bool is_lighting_enabled();
        static void set_ambient_light(const glm::vec3& color, float intensity = 1.0f);
        static void draw_point_light(const point_light_desc& desc);

        // ========================================================================
        // Stats
        // ========================================================================

        [[nodiscard]] static const renderer2d_stats& get_stats();
        static void reset_stats();

    private:
        renderer() = delete;
        ~renderer() = delete;
        renderer(const renderer&) = delete;
        renderer& operator=(const renderer&) = delete;
    };
}
