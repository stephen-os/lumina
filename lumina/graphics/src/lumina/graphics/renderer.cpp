#include "renderer.h"
#include <lumina/core/application.h>

#include <glm/gtc/matrix_transform.hpp>

#include <unordered_map>
#include <cassert>

namespace lumina::graphics
{
    // ============================================================================
    // Static State
    // ============================================================================

    struct renderer_state
    {
        scope<renderer2d> renderer;

        // Context management
        struct context_data
        {
            ref<render_target> target;
            uint32_t width = 0;
            uint32_t height = 0;
        };

        std::unordered_map<uint32_t, context_data> contexts;
        uint32_t next_context_id = 1;  // 0 is reserved for default

        // Default context
        context_data default_context;

        // Current state
        uint32_t current_context_id = 0;
        bool in_frame = false;

        // Default projection (orthographic, top-left origin)
        glm::mat4 current_projection{1.0f};
        bool use_custom_projection = false;
    };

    static renderer_state* s_state = nullptr;

    // ============================================================================
    // Helper Functions
    // ============================================================================

    static glm::mat4 make_default_projection(uint32_t width, uint32_t height)
    {
        return glm::ortho(0.0f, static_cast<float>(width),
                          static_cast<float>(height), 0.0f,
                          -1.0f, 1.0f);
    }

    static renderer_state::context_data* get_context_data(uint32_t id)
    {
        if (id == 0)
        {
            return &s_state->default_context;
        }

        auto it = s_state->contexts.find(id);
        if (it != s_state->contexts.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    // ============================================================================
    // Initialization
    // ============================================================================

    void renderer::init()
    {
        init(800, 600);
    }

    void renderer::init(uint32_t width, uint32_t height)
    {
        assert(s_state == nullptr && "Renderer already initialized");

        s_state = new renderer_state();

        auto& device = core::application::get().get_device();

        // Create renderer2d
        s_state->renderer = make_scope<renderer2d>(device);
        s_state->renderer->init();

        // Create default context
        s_state->default_context.target = render_target::create(
            device, width, height, format::rgba8_unorm
        );
        s_state->default_context.width = width;
        s_state->default_context.height = height;

        // Set default projection
        s_state->current_projection = make_default_projection(width, height);
    }

    void renderer::init(const glm::uvec2& size)
    {
        init(size.x, size.y);
	}

    void renderer::shutdown()
    {
        assert(s_state != nullptr && "Renderer not initialized");

        // Cleanup contexts
        s_state->contexts.clear();
        s_state->default_context.target.reset();

        // Shutdown renderer
        s_state->renderer->shutdown();
        s_state->renderer.reset();

        delete s_state;
        s_state = nullptr;
    }

    bool renderer::is_initialized()
    {
        return s_state != nullptr;
    }

    // ============================================================================
    // Context Management
    // ============================================================================

    render_context renderer::create_context(uint32_t width, uint32_t height)
    {
        assert(s_state != nullptr && "Renderer not initialized");

        auto& device = core::application::get().get_device();

        renderer_state::context_data ctx_data;
        ctx_data.target = render_target::create(device, width, height, format::rgba8_unorm);
        ctx_data.width = width;
        ctx_data.height = height;

        uint32_t id = s_state->next_context_id++;
        s_state->contexts[id] = std::move(ctx_data);

        return render_context{id};
    }

    void renderer::destroy_context(render_context ctx)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        assert(ctx.id != 0 && "Cannot destroy default context");

        s_state->contexts.erase(ctx.id);
    }

    void renderer::resize(uint32_t width, uint32_t height)
    {
        assert(s_state != nullptr && "Renderer not initialized");

        if (width == 0 || height == 0) return;
        if (s_state->default_context.width == width &&
            s_state->default_context.height == height) return;

        auto& device = core::application::get().get_device();

        s_state->default_context.target = render_target::create(
            device, width, height, format::rgba8_unorm
        );
        s_state->default_context.width = width;
        s_state->default_context.height = height;
    }

    void renderer::resize(render_context ctx, uint32_t width, uint32_t height)
    {
        assert(s_state != nullptr && "Renderer not initialized");

        if (ctx.id == 0)
        {
            resize(width, height);
            return;
        }

        if (width == 0 || height == 0) return;

        auto* ctx_data = get_context_data(ctx.id);
        if (!ctx_data) return;

        if (ctx_data->width == width && ctx_data->height == height) return;

        auto& device = core::application::get().get_device();

        ctx_data->target = render_target::create(device, width, height, format::rgba8_unorm);
        ctx_data->width = width;
        ctx_data->height = height;
    }

    glm::uvec2 renderer::get_size()
    {
        assert(s_state != nullptr && "Renderer not initialized");
        return {s_state->default_context.width, s_state->default_context.height};
    }

    glm::uvec2 renderer::get_size(render_context ctx)
    {
        assert(s_state != nullptr && "Renderer not initialized");

        auto* ctx_data = get_context_data(ctx.id);
        if (!ctx_data) return {0, 0};

        return {ctx_data->width, ctx_data->height};
    }

    // ============================================================================
    // Rendering
    // ============================================================================

    void renderer::begin()
    {
        begin(render_context{0});
    }

    void renderer::begin(render_context ctx)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        assert(!s_state->in_frame && "Already in a frame, call end() first");

        auto* ctx_data = get_context_data(ctx.id);
        assert(ctx_data != nullptr && "Invalid render context");

        s_state->current_context_id = ctx.id;
        s_state->in_frame = true;

        // Use custom projection or generate default for this context
        glm::mat4 projection;
        if (s_state->use_custom_projection)
        {
            projection = s_state->current_projection;
        }
        else
        {
            projection = make_default_projection(ctx_data->width, ctx_data->height);
        }

        s_state->renderer->begin(projection);
        s_state->renderer->set_render_target(ctx_data->target);
    }

    void renderer::end()
    {
        assert(s_state != nullptr && "Renderer not initialized");
        assert(s_state->in_frame && "Not in a frame, call begin() first");

        s_state->renderer->end();
        s_state->in_frame = false;

        // Reset custom projection flag for next frame
        s_state->use_custom_projection = false;
    }

    void renderer::clear(const glm::vec4& color)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->clear(color);
    }

    // ============================================================================
    // Camera / Projection
    // ============================================================================

    void renderer::set_camera(const camera2d& camera)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->current_projection = camera.get_view_projection_matrix();
        s_state->use_custom_projection = true;
    }

    void renderer::set_projection(const glm::mat4& projection)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->current_projection = projection;
        s_state->use_custom_projection = true;
    }

    // ============================================================================
    // Output
    // ============================================================================

    ref<texture> renderer::get_texture()
    {
        assert(s_state != nullptr && "Renderer not initialized");

        auto* ctx_data = get_context_data(0);
        if (!ctx_data || !ctx_data->target) return nullptr;

        return ctx_data->target->get_color_texture();
    }

    ref<texture> renderer::get_texture(render_context ctx)
    {
        assert(s_state != nullptr && "Renderer not initialized");

        auto* ctx_data = get_context_data(ctx.id);
        if (!ctx_data || !ctx_data->target) return nullptr;

        return ctx_data->target->get_color_texture();
    }

    // ============================================================================
    // Drawing - Primitives
    // ============================================================================

    void renderer::draw_quad(const quad_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_quad(desc);
    }

    void renderer::draw_circle(const circle_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_circle(desc);
    }

    void renderer::draw_line(const line_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_line(desc);
    }

    void renderer::draw_triangle(const triangle_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_triangle(desc);
    }

    void renderer::draw_rect(const rect_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_rect(desc);
    }

    void renderer::draw_pixel(const pixel_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_pixel(desc);
    }

    void renderer::draw_grid(const grid_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_grid(desc);
    }

    // ============================================================================
    // Drawing - Text
    // ============================================================================

    void renderer::draw_text(const text_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_text(desc);
    }

    void renderer::set_default_font(ref<font_atlas> font)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->set_default_font(font);
    }

    ref<font_atlas> renderer::get_default_font()
    {
        assert(s_state != nullptr && "Renderer not initialized");
        return s_state->renderer->get_default_font();
    }

    // ============================================================================
    // Drawing - Sprites
    // ============================================================================

    void renderer::draw_sprite(const texture_atlas& atlas, const std::string& region_name, const sprite_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_sprite(atlas, region_name, desc);
    }

    void renderer::draw_sprite(const texture_atlas& atlas, uint32_t region_index, const sprite_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_sprite(atlas, region_index, desc);
    }

    void renderer::draw_sprite(const atlas_region& region, ref<texture> atlas_texture, const sprite_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_sprite(region, atlas_texture, desc);
    }

    // ============================================================================
    // Scissor / Clipping
    // ============================================================================

    void renderer::push_scissor(float x, float y, float width, float height)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->push_scissor(x, y, width, height);
    }

    void renderer::push_scissor(const glm::vec4& rect)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->push_scissor(rect);
    }

    void renderer::pop_scissor()
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->pop_scissor();
    }

    bool renderer::has_scissor()
    {
        assert(s_state != nullptr && "Renderer not initialized");
        return s_state->renderer->has_scissor();
    }

    // ============================================================================
    // Texture Filtering
    // ============================================================================

    void renderer::set_filter_mode(filter_mode mode)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->set_filter_mode(mode);
    }

    filter_mode renderer::get_filter_mode()
    {
        assert(s_state != nullptr && "Renderer not initialized");
        return s_state->renderer->get_filter_mode();
    }

    // ============================================================================
    // Lighting
    // ============================================================================

    void renderer::set_lighting_enabled(bool enabled)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->set_lighting_enabled(enabled);
    }

    bool renderer::is_lighting_enabled()
    {
        assert(s_state != nullptr && "Renderer not initialized");
        return s_state->renderer->is_lighting_enabled();
    }

    void renderer::set_ambient_light(const glm::vec3& color, float intensity)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->set_ambient_light(color, intensity);
    }

    void renderer::draw_point_light(const point_light_desc& desc)
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->draw_point_light(desc);
    }

    // ============================================================================
    // Stats
    // ============================================================================

    const renderer2d_stats& renderer::get_stats()
    {
        assert(s_state != nullptr && "Renderer not initialized");
        return s_state->renderer->get_stats();
    }

    void renderer::reset_stats()
    {
        assert(s_state != nullptr && "Renderer not initialized");
        s_state->renderer->reset_stats();
    }
}
