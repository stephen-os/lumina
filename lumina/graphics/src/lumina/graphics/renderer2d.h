#pragma once

#include "types.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "uniform_buffer.h"
#include "texture.h"
#include "sampler.h"
#include "shader.h"
#include "input_layout.h"
#include "binding_layout.h"
#include "pipeline.h"
#include "render_target.h"
#include "font_atlas.h"
#include "camera2d.h"
#include "texture_atlas.h"

#include <lumina/core/base.h>

#include <glm/glm.hpp>

#include <array>
#include <map>
#include <stack>
#include <string>
#include <string_view>

namespace lumina::core { class device; }

namespace lumina::graphics
{
    class context;

    // ============================================================================
    // Enums
    // ============================================================================

    // Render layers control draw order. Lower values render first (background).
    // Users can use any uint32_t value for custom layers.
    enum class render_layer : uint32_t
    {
        background = 0,
        world = 100,
        sprites = 200,
        effects = 300,
        ui = 400,
        overlay = 500,

        // Default layer
        default_layer = world
    };

    // Helper to create custom layer values
    constexpr render_layer make_layer(uint32_t value) { return static_cast<render_layer>(value); }

    enum class text_alignment
    {
        left,
        center,
        right
    };

    enum class light_blend_mode
    {
        additive,
        multiply,
        screen,
        overlay,
        soft_light,
        linear_burn,
        color_dodge,
        subtract,
        alpha
    };

    enum class attenuation_model
    {
        none,
        linear,
        quadratic,
        inverse_square,
        exponential,
        smoothstep,
        realistic
    };

    // ============================================================================
    // Descriptor Structs
    // ============================================================================

    struct quad_desc
    {
        glm::vec3 position = {0, 0, 0};
        glm::vec2 size = {1, 1};
        glm::vec4 color = {1, 1, 1, 1};
        float rotation = 0.0f;
        glm::vec2 origin = {0.5f, 0.5f};    // Rotation pivot (0-1 normalized)
        ref<texture> texture = nullptr;
        glm::vec2 uv_min = {0, 0};
        glm::vec2 uv_max = {1, 1};
        render_layer layer = render_layer::default_layer;
        float z = 0.0f;                     // Depth within layer (0.0-1.0)
        blend_mode blend = blend_mode::alpha;
    };

    struct circle_desc
    {
        glm::vec3 position = {0, 0, 0};
        glm::vec2 radius = {1, 1};          // x,y for ellipse support
        glm::vec4 color = {1, 1, 1, 1};
        float thickness = 1.0f;             // 1.0 = filled, <1.0 = ring
        float fade = 0.005f;                // Soft edge fade
        ref<texture> texture = nullptr;
        glm::vec2 uv_min = {0, 0};
        glm::vec2 uv_max = {1, 1};
        render_layer layer = render_layer::default_layer;
        float z = 0.0f;                     // Depth within layer (0.0-1.0)
        blend_mode blend = blend_mode::alpha;
    };

    struct line_desc
    {
        glm::vec3 start = {0, 0, 0};
        glm::vec3 end = {1, 0, 0};
        glm::vec4 color = {1, 1, 1, 1};
        float thickness = 1.0f;
        render_layer layer = render_layer::default_layer;
        float z = 0.0f;                     // Depth within layer (0.0-1.0)
    };

    struct text_desc
    {
        std::string_view text;
        glm::vec3 position = {0, 0, 0};
        float scale = 1.0f;                 // Scale factor (1.0 = font's native pixel height)
        glm::vec4 color = {1, 1, 1, 1};
        text_alignment alignment = text_alignment::left;
        ref<font_atlas> font = nullptr;     // nullptr = default font
        render_layer layer = render_layer::default_layer;
        float z = 0.0f;                     // Depth within layer (0.0-1.0)
    };

    struct triangle_desc
    {
        glm::vec3 p0 = {0, 0, 0};
        glm::vec3 p1 = {1, 0, 0};
        glm::vec3 p2 = {0.5f, 1, 0};
        glm::vec4 color = {1, 1, 1, 1};
        ref<texture> texture = nullptr;
        glm::vec2 uv0 = {0, 1};
        glm::vec2 uv1 = {1, 1};
        glm::vec2 uv2 = {0.5f, 0};
        render_layer layer = render_layer::default_layer;
        float z = 0.0f;                     // Depth within layer (0.0-1.0)
        blend_mode blend = blend_mode::alpha;
    };

    struct pixel_desc
    {
        glm::vec3 position = {0, 0, 0};
        glm::vec4 color = {1, 1, 1, 1};
        float size = 1.0f;
        render_layer layer = render_layer::default_layer;
        float z = 0.0f;                     // Depth within layer (0.0-1.0)
    };

    struct rect_desc
    {
        glm::vec3 position = {0, 0, 0};
        glm::vec2 size = {1, 1};
        glm::vec4 color = {1, 1, 1, 1};
        float thickness = 1.0f;
        float rotation = 0.0f;
        render_layer layer = render_layer::default_layer;
        float z = 0.0f;                     // Depth within layer (0.0-1.0)
    };

    struct grid_desc
    {
        glm::vec3 position = {0, 0, 0};
        glm::vec2 size = {10, 10};
        float cell_size = 1.0f;
        glm::vec4 line_color = {0.5f, 0.5f, 0.5f, 1.0f};
        float line_width = 1.0f;
        bool checkerboard = false;
        glm::vec4 checker_color1 = {0.4f, 0.4f, 0.4f, 1.0f};
        glm::vec4 checker_color2 = {0.6f, 0.6f, 0.6f, 1.0f};
        render_layer layer = render_layer::default_layer;
        float z = 0.0f;                     // Depth within layer (0.0-1.0)
    };

    struct point_light_desc
    {
        glm::vec3 position = {0, 0, 0};
        glm::vec3 color = {1, 1, 1};
        float intensity = 1.0f;
        float radius = 10.0f;
        light_blend_mode blend = light_blend_mode::additive;
        float blend_alpha = 1.0f;
        attenuation_model attenuation = attenuation_model::quadratic;
        float falloff = 1.0f;
        render_layer layer = render_layer::effects;
    };

    struct sprite_desc
    {
        glm::vec3 position = {0, 0, 0};
        glm::vec2 size = {0, 0};            // If {0,0}, uses region size
        glm::vec4 color = {1, 1, 1, 1};
        float rotation = 0.0f;
        glm::vec2 origin = {0.5f, 0.5f};    // Rotation pivot (0-1 normalized)
        bool flip_x = false;                 // Horizontal flip
        bool flip_y = false;                 // Vertical flip
        render_layer layer = render_layer::default_layer;
        float z = 0.0f;                     // Depth within layer (0.0-1.0)
        blend_mode blend = blend_mode::alpha;
    };

    // ============================================================================
    // Statistics
    // ============================================================================

    struct renderer2d_stats
    {
        uint32_t draw_calls = 0;
        uint32_t quad_count = 0;
        uint32_t circle_count = 0;
        uint32_t line_count = 0;
        uint32_t text_char_count = 0;
        uint32_t triangle_count = 0;
        uint32_t pixel_count = 0;
        uint32_t grid_count = 0;
        uint32_t point_light_count = 0;
        uint32_t texture_binds = 0;

        uint32_t get_total_primitives() const
        {
            return quad_count + circle_count + line_count + triangle_count + pixel_count + grid_count;
        }

        void reset()
        {
            draw_calls = 0;
            quad_count = 0;
            circle_count = 0;
            line_count = 0;
            text_char_count = 0;
            triangle_count = 0;
            pixel_count = 0;
            grid_count = 0;
            point_light_count = 0;
            texture_binds = 0;
        }
    };

    // ============================================================================
    // Configuration
    // ============================================================================

    struct renderer2d_config
    {
        uint32_t max_quads = 10000;
        uint32_t max_circles = 10000;
        uint32_t max_lines = 10000;
        uint32_t max_triangles = 10000;
        uint32_t max_text_chars = 10000;
        uint32_t max_pixels = 50000;
        uint32_t max_grids = 1000;
        uint32_t max_point_lights = 32;
        uint32_t max_textures = 32;
    };

    // ============================================================================
    // Renderer2D Class
    // ============================================================================

    class renderer2d
    {
    public:
        explicit renderer2d(core::device& dev);
        ~renderer2d();

        renderer2d(const renderer2d&) = delete;
        renderer2d& operator=(const renderer2d&) = delete;

        // Initialization
        bool init(const renderer2d_config& config = {});
        void shutdown();

        // Scene management
        void begin(const glm::mat4& projection);
        void begin(const glm::mat4& view, const glm::mat4& projection);
        void begin(const camera2d& camera);
        void end();

        // Render target
        void set_render_target(ref<render_target> target);
        void set_default_render_target();
        void clear(const glm::vec4& color = {0.0f, 0.0f, 0.0f, 1.0f});

        // ========================================================================
        // Drawing API (struct-based)
        // ========================================================================

        void draw_quad(const quad_desc& desc);
        void draw_circle(const circle_desc& desc);
        void draw_line(const line_desc& desc);
        void draw_text(const text_desc& desc);
        void set_default_font(ref<font_atlas> font) { m_default_font = font; }
        ref<font_atlas> get_default_font() const { return m_default_font; }
        void draw_triangle(const triangle_desc& desc);
        void draw_pixel(const pixel_desc& desc);
        void draw_rect(const rect_desc& desc);
        void draw_grid(const grid_desc& desc);

        // Sprite drawing (texture atlas support)
        void draw_sprite(const texture_atlas& atlas, const std::string& region_name, const sprite_desc& desc);
        void draw_sprite(const texture_atlas& atlas, uint32_t region_index, const sprite_desc& desc);
        void draw_sprite(const atlas_region& region, ref<texture> atlas_texture, const sprite_desc& desc);

        // ========================================================================
        // Scissor/Clipping Regions
        // ========================================================================

        // Push a scissor rectangle onto the stack. All subsequent drawing will be clipped to this region.
        // If there's already a scissor active, the new scissor is intersected with the current one.
        // x, y = top-left corner in screen coordinates, w, h = dimensions
        void push_scissor(float x, float y, float width, float height);
        void push_scissor(const glm::vec4& rect);  // rect = {x, y, width, height}

        // Pop the current scissor rectangle, restoring the previous one (or disabling scissor if stack is empty)
        void pop_scissor();

        // Check if scissor is currently active
        bool has_scissor() const { return !m_scissor_stack.empty(); }

        // Get the current scissor rect (returns zero rect if no scissor active)
        glm::vec4 get_current_scissor() const;

        // ========================================================================
        // Texture Filtering
        // ========================================================================

        void set_filter_mode(filter_mode mode);
        filter_mode get_filter_mode() const { return m_filter_mode; }

        // ========================================================================
        // Lighting
        // ========================================================================

        void set_lighting_enabled(bool enabled);
        bool is_lighting_enabled() const { return m_lighting_enabled; }

        void set_ambient_light(const glm::vec3& color, float intensity = 1.0f);
        const glm::vec3& get_ambient_color() const { return m_ambient_color; }
        float get_ambient_intensity() const { return m_ambient_intensity; }

        void draw_point_light(const point_light_desc& desc);

        // ========================================================================
        // Stats
        // ========================================================================

        const renderer2d_stats& get_stats() const { return m_stats; }
        void reset_stats() { m_stats.reset(); }

    private:
        core::device& m_device;
        lumina::scope<context> m_context;
        lumina::scope<pipeline_cache> m_pipeline_cache;
        std::vector<ref<binding_set>> m_frame_binding_sets;  // Keep binding sets alive during frame
        renderer2d_config m_config;
        bool m_initialized = false;

        // Camera
        glm::mat4 m_view_projection{1.0f};

        // Scissor stack
        std::stack<glm::vec4> m_scissor_stack;  // Stack of scissor rects {x, y, width, height}

        // ========================================================================
        // Vertex Types (internal)
        // ========================================================================

        struct quad_vertex
        {
            glm::vec4 position;     // World position (xyz) + padding
            glm::vec4 color;
            glm::vec2 texcoord;
            float tex_index;
            float z_index;          // Depth within layer (0.0-1.0)
        };

        struct circle_vertex
        {
            glm::vec4 world_position;   // World position (xyz) + padding
            glm::vec4 local_position;   // Local coords for SDF
            glm::vec4 color;
            glm::vec2 texcoord;
            float tex_index;
            float thickness;
            float fade;
            float z_index;              // Depth within layer (0.0-1.0)
            float _pad[2];
        };

        struct line_vertex
        {
            glm::vec4 position;     // World position (xyz) + z_index in w
            glm::vec4 color;
        };

        struct text_vertex
        {
            glm::vec4 position;     // World position (xyz) + padding
            glm::vec4 color;
            glm::vec2 texcoord;
            float tex_index;
            float z_index;          // Depth within layer (0.0-1.0)
        };

        struct triangle_vertex
        {
            glm::vec4 position;     // World position (xyz) + padding
            glm::vec4 color;
            glm::vec2 texcoord;
            float tex_index;
            float z_index;          // Depth within layer (0.0-1.0)
        };

        struct pixel_vertex
        {
            glm::vec4 position;     // World position (xyz) + z_index in w
            glm::vec4 color;
            float size;
            float _pad[3];
        };

        struct grid_vertex
        {
            glm::vec4 position;         // World position (xyz) + z_index in w
            glm::vec4 local_position;   // For procedural grid in shader
            glm::vec4 line_color;
            glm::vec2 grid_size;
            float cell_size;
            float line_width;
            float show_checkerboard;
            glm::vec4 checker_color1;
            glm::vec4 checker_color2;
        };

        // ========================================================================
        // Layer Batch (per-layer vertex storage)
        // ========================================================================

        struct layer_batch
        {
            std::vector<quad_vertex> quad_vertices;
            std::vector<circle_vertex> circle_vertices;
            std::vector<line_vertex> line_vertices;
            std::vector<text_vertex> text_vertices;
            std::vector<triangle_vertex> triangle_vertices;
            std::vector<pixel_vertex> pixel_vertices;
            std::vector<grid_vertex> grid_vertices;

            uint32_t quad_count = 0;
            uint32_t circle_count = 0;
            uint32_t line_count = 0;
            uint32_t text_char_count = 0;
            uint32_t triangle_count = 0;
            uint32_t pixel_count = 0;
            uint32_t grid_count = 0;

            // Texture batching per layer
            std::array<ref<texture>, 32> texture_slots;
            uint32_t texture_slot_index = 0;

            // Blend mode tracking per primitive type
            blend_mode quad_blend = blend_mode::alpha;
            blend_mode circle_blend = blend_mode::alpha;
            blend_mode triangle_blend = blend_mode::alpha;

            void clear()
            {
                quad_vertices.clear();
                circle_vertices.clear();
                line_vertices.clear();
                text_vertices.clear();
                triangle_vertices.clear();
                pixel_vertices.clear();
                grid_vertices.clear();

                quad_count = 0;
                circle_count = 0;
                line_count = 0;
                text_char_count = 0;
                triangle_count = 0;
                pixel_count = 0;
                grid_count = 0;

                texture_slots.fill(nullptr);
                texture_slot_index = 0;

                quad_blend = blend_mode::alpha;
                circle_blend = blend_mode::alpha;
                triangle_blend = blend_mode::alpha;
            }

            bool is_empty() const
            {
                return quad_count == 0 && circle_count == 0 && line_count == 0 &&
                       text_char_count == 0 && triangle_count == 0 && pixel_count == 0 &&
                       grid_count == 0;
            }
        };

        // ========================================================================
        // Batch Limits
        // ========================================================================

        static constexpr uint32_t vertices_per_quad = 4;
        static constexpr uint32_t indices_per_quad = 6;

        // ========================================================================
        // Per-Layer Batches (sorted map for ordered iteration)
        // ========================================================================

        std::map<uint32_t, layer_batch> m_layers;

        // ========================================================================
        // Private Methods
        // ========================================================================

        void flush_layer(uint32_t layer_id);
        void flush_layers_up_to(uint32_t layer_id);  // Flush all layers below this layer
        void flush_quads(uint32_t layer_id);
        void flush_circles(uint32_t layer_id);
        void flush_lines(uint32_t layer_id);
        void flush_text(uint32_t layer_id);
        void flush_triangles(uint32_t layer_id);
        void flush_pixels(uint32_t layer_id);
        void flush_grids(uint32_t layer_id);
        void flush_all();

        void start_batch();
        float get_texture_index(uint32_t layer_id, ref<texture> tex);
        layer_batch& get_layer(uint32_t layer_id);
        void apply_scissor();  // Apply current scissor state to context

        // ========================================================================
        // Shared GPU Resources (pipelines, shaders, buffers - shared across layers)
        // ========================================================================

        // Quad
        ref<vertex_buffer> m_quad_vertex_buffer;
        ref<index_buffer> m_quad_index_buffer;
        ref<shader> m_quad_shader;
        ref<input_layout> m_quad_input_layout;
        ref<binding_layout> m_quad_binding_layout;
        ref<pipeline> m_quad_pipeline;
        uint32_t m_quad_vertex_offset = 0;  // Current vertex offset for appending (in vertices, not bytes)

        // Circle
        ref<vertex_buffer> m_circle_vertex_buffer;
        ref<index_buffer> m_circle_index_buffer;
        ref<shader> m_circle_shader;
        ref<input_layout> m_circle_input_layout;
        ref<binding_layout> m_circle_binding_layout;
        ref<pipeline> m_circle_pipeline;
        uint32_t m_circle_vertex_offset = 0;

        // Line
        ref<vertex_buffer> m_line_vertex_buffer;
        ref<shader> m_line_shader;
        ref<input_layout> m_line_input_layout;
        ref<binding_layout> m_line_binding_layout;
        ref<pipeline> m_line_pipeline;
        uint32_t m_line_vertex_offset = 0;

        // Text
        ref<vertex_buffer> m_text_vertex_buffer;
        ref<index_buffer> m_text_index_buffer;
        ref<shader> m_text_shader;
        ref<input_layout> m_text_input_layout;
        ref<binding_layout> m_text_binding_layout;
        ref<pipeline> m_text_pipeline;
        ref<font_atlas> m_default_font;
        uint32_t m_text_vertex_offset = 0;

        // Triangle
        ref<vertex_buffer> m_triangle_vertex_buffer;
        ref<shader> m_triangle_shader;
        ref<input_layout> m_triangle_input_layout;
        ref<binding_layout> m_triangle_binding_layout;
        ref<pipeline> m_triangle_pipeline;
        uint32_t m_triangle_vertex_offset = 0;

        // Pixel
        ref<vertex_buffer> m_pixel_vertex_buffer;
        ref<shader> m_pixel_shader;
        ref<input_layout> m_pixel_input_layout;
        ref<binding_layout> m_pixel_binding_layout;
        ref<pipeline> m_pixel_pipeline;
        uint32_t m_pixel_vertex_offset = 0;

        // Grid
        ref<vertex_buffer> m_grid_vertex_buffer;
        ref<index_buffer> m_grid_index_buffer;
        ref<shader> m_grid_shader;
        ref<input_layout> m_grid_input_layout;
        ref<binding_layout> m_grid_binding_layout;
        ref<pipeline> m_grid_pipeline;
        uint32_t m_grid_vertex_offset = 0;

        // ========================================================================
        // Shared Resources
        // ========================================================================

        ref<sampler> m_default_sampler;
        ref<sampler> m_point_sampler;
        ref<sampler> m_current_sampler;  // Active sampler (linear or point)
        filter_mode m_filter_mode = filter_mode::linear;
        ref<texture> m_white_texture;

        // Camera constant buffer (shared by all primitives)
        struct camera_data
        {
            glm::mat4 view_projection;
        };
        ref<uniform_buffer> m_camera_buffer;

        // ========================================================================
        // Lighting State
        // ========================================================================

        bool m_lighting_enabled = false;
        glm::vec3 m_ambient_color{0.1f, 0.1f, 0.1f};
        float m_ambient_intensity = 1.0f;

        struct point_light_data
        {
            glm::vec3 position;
            float intensity;
            glm::vec3 color;
            float radius;
            float blend_mode;
            float blend_alpha;
            float attenuation;
            float falloff;
        };

        std::vector<point_light_data> m_point_lights;

        // ========================================================================
        // Lighting Resources (Godot-style deferred 2D lighting)
        // ========================================================================

        // Render targets for lighting pass
        ref<render_target> m_scene_target;          // Scene is rendered here when lighting enabled
        ref<render_target> m_light_accumulation;    // Light contributions accumulated here

        // Fullscreen quad for lighting passes
        struct fullscreen_vertex
        {
            glm::vec4 position;
            glm::vec2 texcoord;
        };
        ref<vertex_buffer> m_fullscreen_vertex_buffer;

        // Point light shader and pipeline
        ref<shader> m_point_light_shader;
        ref<input_layout> m_point_light_input_layout;
        ref<binding_layout> m_point_light_binding_layout;
        ref<pipeline> m_point_light_pipeline;

        // Composite shader and pipeline
        ref<shader> m_composite_shader;
        ref<input_layout> m_composite_input_layout;
        ref<binding_layout> m_composite_binding_layout;
        ref<pipeline> m_composite_pipeline;

        // Constant buffers for lighting passes
        ref<uniform_buffer> m_light_params_buffer;
        ref<uniform_buffer> m_composite_params_buffer;

        // Cached viewport size for lighting target resize
        uint32_t m_lighting_target_width = 0;
        uint32_t m_lighting_target_height = 0;

        // Private lighting methods
        void init_lighting_resources();
        void shutdown_lighting_resources();
        void ensure_lighting_targets(uint32_t width, uint32_t height);
        void flush_lights();
        void composite_scene();

        // ========================================================================
        // Render Target State
        // ========================================================================

        ref<render_target> m_current_target;
        format m_current_color_format = format::rgba8_unorm;
        format m_current_depth_format = format::unknown;

        // Stats
        renderer2d_stats m_stats;
    };
}
