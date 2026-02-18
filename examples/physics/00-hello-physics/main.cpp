// physics/00-hello-physics: Basic Physics Demo
// Demonstrates: Physics world, dynamic/static bodies, basic collision

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/core/input.h>
#include <lumina/graphics/graphics.h>
#include <lumina/physics/physics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <vector>
#include <random>

namespace ui = lumina::ui;
namespace gfx = lumina::graphics;
namespace input = lumina::core::input;
namespace physics = lumina::physics;

// Scale factor: pixels per meter (Box2D works in meters)
constexpr float PIXELS_PER_METER = 50.0f;

// Convert physics coords to render coords
inline glm::vec2 to_render(const glm::vec2& physics_pos, float render_size)
{
    return {
        physics_pos.x * PIXELS_PER_METER,
        render_size - (physics_pos.y * PIXELS_PER_METER)  // Flip Y axis
    };
}

// Convert render coords to physics coords
inline glm::vec2 to_physics(const glm::vec2& render_pos, float render_size)
{
    return {
        render_pos.x / PIXELS_PER_METER,
        (render_size - render_pos.y) / PIXELS_PER_METER
    };
}

// Simple body wrapper with color
struct colored_body
{
    lumina::ref<physics::body> body;
    glm::vec4 color;
    bool is_circle = false;
    float half_width = 0.5f;
    float half_height = 0.5f;
    float radius = 0.5f;
};

class hello_physics_layer : public lumina::core::layer
{
public:
    hello_physics_layer() : layer("hello_physics") {}

    void on_attach() override
    {
        gfx::renderer::init(600, 600, 4);

        // Create physics world with gravity pointing down
        physics::world_def world_def;
        world_def.gravity = {0.0f, -9.81f};
        m_world = std::make_unique<physics::world>(world_def);

        // Create ground (static body)
        create_ground();

        // Create some initial boxes
        for (int i = 0; i < 5; i++)
        {
            spawn_box({3.0f + i * 1.2f, 8.0f + i * 0.5f});
        }

        LUMINA_LOG_INFO("Hello Physics demo loaded");
    }

    void on_detach() override
    {
        m_bodies.clear();
        m_world.reset();
        gfx::renderer::shutdown();
    }

    void on_update(float dt) override
    {
        // Step physics
        if (!m_paused)
        {
            m_world->step(dt);
        }

        // Spawn with mouse click (only when viewport is hovered)
        if (input::is_mouse_button_pressed(input::mouse_code::left))
        {
            if (!m_mouse_down && m_viewport_hovered)
            {
                // Convert viewport-relative position to physics position
                glm::vec2 physics_pos = to_physics(m_viewport_mouse_pos, 600.0f);

                if (m_spawn_circles)
                    spawn_circle(physics_pos);
                else
                    spawn_box(physics_pos);
            }
            m_mouse_down = true;
        }
        else
        {
            m_mouse_down = false;
        }

        // Reset with R key
        if (input::is_key_pressed(input::key_code::r))
        {
            if (!m_r_down)
            {
                reset_simulation();
            }
            m_r_down = true;
        }
        else
        {
            m_r_down = false;
        }

        // Pause with space
        if (input::is_key_pressed(input::key_code::space))
        {
            if (!m_space_down)
            {
                m_paused = !m_paused;
            }
            m_space_down = true;
        }
        else
        {
            m_space_down = false;
        }
    }

    void on_render() override
    {
        const float render_size = 600.0f;

        gfx::renderer::begin();
        gfx::renderer::clear({0.1f, 0.1f, 0.12f, 1.0f});

        // Draw ground
        glm::vec2 ground_render = to_render(m_ground_pos, render_size);
        gfx::renderer::draw_quad({
            .position = {ground_render.x, ground_render.y, 0},
            .size = {m_ground_width * PIXELS_PER_METER * 2, m_ground_height * PIXELS_PER_METER * 2},
            .color = {0.3f, 0.3f, 0.35f, 1.0f}
        });

        // Draw walls
        glm::vec2 left_wall_render = to_render(m_left_wall_pos, render_size);
        gfx::renderer::draw_quad({
            .position = {left_wall_render.x, left_wall_render.y, 0},
            .size = {m_wall_thickness * PIXELS_PER_METER * 2, m_wall_height * PIXELS_PER_METER * 2},
            .color = {0.3f, 0.3f, 0.35f, 1.0f}
        });

        glm::vec2 right_wall_render = to_render(m_right_wall_pos, render_size);
        gfx::renderer::draw_quad({
            .position = {right_wall_render.x, right_wall_render.y, 0},
            .size = {m_wall_thickness * PIXELS_PER_METER * 2, m_wall_height * PIXELS_PER_METER * 2},
            .color = {0.3f, 0.3f, 0.35f, 1.0f}
        });

        // Draw dynamic bodies
        for (const auto& cb : m_bodies)
        {
            if (!cb.body || !cb.body->is_valid()) continue;

            glm::vec2 pos = cb.body->get_position();
            float rotation = cb.body->get_rotation();
            glm::vec2 render_pos = to_render(pos, render_size);

            if (cb.is_circle)
            {
                gfx::renderer::draw_circle({
                    .position = {render_pos.x, render_pos.y, 0},
                    .radius = {cb.radius * PIXELS_PER_METER, cb.radius * PIXELS_PER_METER},
                    .color = cb.color
                });

                // Draw a line to show rotation
                float line_len = cb.radius * PIXELS_PER_METER * 0.8f;
                glm::vec2 line_end = {
                    render_pos.x + std::cos(-rotation) * line_len,
                    render_pos.y + std::sin(-rotation) * line_len
                };
                gfx::renderer::draw_line({
                    .start = {render_pos.x, render_pos.y, 0.1f},
                    .end = {line_end.x, line_end.y, 0.1f},
                    .color = {1, 1, 1, 0.8f},
                    .thickness = 2.0f
                });
            }
            else
            {
                gfx::renderer::draw_quad({
                    .position = {render_pos.x, render_pos.y, 0},
                    .size = {cb.half_width * PIXELS_PER_METER * 2, cb.half_height * PIXELS_PER_METER * 2},
                    .color = cb.color,
                    .rotation = -rotation  // Flip rotation for render coords
                });
            }
        }

        // Draw title
        gfx::renderer::draw_text({
            .text = "Hello Physics!",
            .position = {300, 30, 0},
            .scale = 1.5f,
            .color = {1, 1, 1, 1},
            .alignment = gfx::text_alignment::center
        });

        // Draw instructions
        gfx::renderer::draw_text({
            .text = "Click to spawn | R = Reset | Space = Pause",
            .position = {300, 580, 0},
            .scale = 0.9f,
            .color = {0.6f, 0.6f, 0.6f, 1},
            .alignment = gfx::text_alignment::center
        });

        gfx::renderer::end();

        // UI
        render_ui();
    }

    void render_ui()
    {
        ui::begin_window("Physics Controls");
        ui::text("Hello Physics Demo");
        ui::separator();

        ui::text_fmt("Bodies: {}", m_bodies.size());
        ui::text_fmt("Paused: {}", m_paused ? "Yes" : "No");
        ui::separator();

        ui::text("Spawn Settings:");
        ui::checkbox("Spawn Circles", m_spawn_circles);
        ui::property_slider("Box Size", m_spawn_size, 0.3f, 1.5f);
        ui::property_slider("Restitution", m_spawn_restitution, 0.0f, 1.0f);
        ui::separator();

        ui::text("World Settings:");
        glm::vec2 gravity = m_world->get_gravity();
        if (ui::property_slider("Gravity Y", gravity.y, -20.0f, 20.0f))
        {
            m_world->set_gravity(gravity);
        }
        ui::separator();

        if (ui::button("Spawn Box"))
        {
            spawn_box({6.0f, 10.0f});
        }
        ui::same_line();
        if (ui::button("Spawn Circle"))
        {
            spawn_circle({6.0f, 10.0f});
        }

        if (ui::button("Spawn 10 Random"))
        {
            for (int i = 0; i < 10; i++)
            {
                float x = 2.0f + m_rng() % 80 / 10.0f;
                float y = 8.0f + m_rng() % 30 / 10.0f;
                if (m_rng() % 2 == 0)
                    spawn_box({x, y});
                else
                    spawn_circle({x, y});
            }
        }
        ui::same_line();
        if (ui::button("Clear All"))
        {
            m_bodies.clear();
        }

        ui::separator();
        const auto& stats = gfx::renderer::get_stats();
        ui::text_fmt("Draw Calls: {}", stats.draw_calls);
        gfx::renderer::reset_stats();

        ui::end_window();

        // Viewport - maintain 1:1 aspect ratio
        ui::begin_window("Viewport");
        auto tex = gfx::renderer::get_texture();
        if (tex)
        {
            auto content_size = ui::get_content_size();
            float size = std::min(content_size.x, content_size.y);
            auto img_rect = ui::image_with_rect(tex->get_texture(), size, size);

            // Track viewport hover and mouse position for spawning
            m_viewport_hovered = ui::is_item_hovered();
            if (m_viewport_hovered)
            {
                auto mouse_pos = ui::get_mouse_pos();
                // Calculate position relative to viewport image, scaled to render target size
                float scale = 600.0f / size;
                m_viewport_mouse_pos = {
                    (mouse_pos.x - img_rect.pos.x) * scale,
                    (mouse_pos.y - img_rect.pos.y) * scale
                };
            }
        }
        ui::end_window();
    }

private:
    void create_ground()
    {
        // Ground
        physics::body_def ground_def;
        ground_def.type = physics::body_type::static_body;
        ground_def.position = m_ground_pos;
        auto ground = m_world->create_body(ground_def);
        ground->add_box(m_ground_width, m_ground_height);

        // Left wall
        physics::body_def left_wall_def;
        left_wall_def.type = physics::body_type::static_body;
        left_wall_def.position = m_left_wall_pos;
        auto left_wall = m_world->create_body(left_wall_def);
        left_wall->add_box(m_wall_thickness, m_wall_height);

        // Right wall
        physics::body_def right_wall_def;
        right_wall_def.type = physics::body_type::static_body;
        right_wall_def.position = m_right_wall_pos;
        auto right_wall = m_world->create_body(right_wall_def);
        right_wall->add_box(m_wall_thickness, m_wall_height);
    }

    void spawn_box(const glm::vec2& pos)
    {
        physics::body_def def;
        def.type = physics::body_type::dynamic;
        def.position = pos;
        def.rotation = (m_rng() % 100) / 100.0f * 0.5f;  // Small random rotation

        auto body = m_world->create_body(def);

        physics::shape_def shape_def;
        shape_def.material.restitution = m_spawn_restitution;
        shape_def.material.friction = 0.5f;
        shape_def.material.density = 1.0f;

        float half_size = m_spawn_size * 0.5f;
        body->add_box(half_size, half_size, {}, 0.0f, shape_def);

        colored_body cb;
        cb.body = body;
        cb.color = random_color();
        cb.is_circle = false;
        cb.half_width = half_size;
        cb.half_height = half_size;

        m_bodies.push_back(cb);
    }

    void spawn_circle(const glm::vec2& pos)
    {
        physics::body_def def;
        def.type = physics::body_type::dynamic;
        def.position = pos;

        auto body = m_world->create_body(def);

        physics::shape_def shape_def;
        shape_def.material.restitution = m_spawn_restitution;
        shape_def.material.friction = 0.5f;
        shape_def.material.density = 1.0f;

        float radius = m_spawn_size * 0.5f;
        body->add_circle(radius, {}, shape_def);

        colored_body cb;
        cb.body = body;
        cb.color = random_color();
        cb.is_circle = true;
        cb.radius = radius;

        m_bodies.push_back(cb);
    }

    void reset_simulation()
    {
        m_bodies.clear();

        // Recreate world
        physics::world_def world_def;
        world_def.gravity = m_world->get_gravity();
        m_world = std::make_unique<physics::world>(world_def);

        create_ground();

        // Spawn initial boxes
        for (int i = 0; i < 5; i++)
        {
            spawn_box({3.0f + i * 1.2f, 8.0f + i * 0.5f});
        }
    }

    glm::vec4 random_color()
    {
        float h = (m_rng() % 100) / 100.0f;
        float s = 0.6f + (m_rng() % 40) / 100.0f;
        float v = 0.7f + (m_rng() % 30) / 100.0f;

        // HSV to RGB
        float c = v * s;
        float x = c * (1 - std::abs(std::fmod(h * 6.0f, 2.0f) - 1));
        float m = v - c;
        float r, g, b;
        if (h < 1.0f/6.0f) { r = c; g = x; b = 0; }
        else if (h < 2.0f/6.0f) { r = x; g = c; b = 0; }
        else if (h < 3.0f/6.0f) { r = 0; g = c; b = x; }
        else if (h < 4.0f/6.0f) { r = 0; g = x; b = c; }
        else if (h < 5.0f/6.0f) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }

        return {r + m, g + m, b + m, 1.0f};
    }

    std::unique_ptr<physics::world> m_world;
    std::vector<colored_body> m_bodies;

    // Ground dimensions (in meters) - 12m x 12m world
    glm::vec2 m_ground_pos = {6.0f, 0.5f};
    float m_ground_width = 5.5f;
    float m_ground_height = 0.5f;

    // Wall dimensions
    glm::vec2 m_left_wall_pos = {0.5f, 6.0f};
    glm::vec2 m_right_wall_pos = {11.5f, 6.0f};
    float m_wall_thickness = 0.5f;
    float m_wall_height = 6.0f;

    // Spawn settings
    bool m_spawn_circles = false;
    float m_spawn_size = 0.8f;
    float m_spawn_restitution = 0.3f;

    // State
    bool m_paused = false;
    bool m_mouse_down = false;
    bool m_r_down = false;
    bool m_space_down = false;

    // Viewport tracking
    bool m_viewport_hovered = false;
    glm::vec2 m_viewport_mouse_pos{0, 0};

    std::minstd_rand m_rng{42};
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    application_specifications specs;
    specs.title = "physics/00-hello-physics";
    auto* app = new application(specs);
    app->push_layer<hello_physics_layer>();
    return app;
}
