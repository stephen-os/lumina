// 01-primitives: 2D primitive rendering
// Demonstrates: Quads, circles, lines, triangles, rectangles, pixels, grids, camera controls

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/core/input.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ui = lumina::ui;
namespace input = lumina::core::input;

class primitives_layer : public lumina::core::layer
{
public:
    primitives_layer() : layer("primitives") {}

    void on_attach() override
    {
        auto& device = lumina::core::application::get().get_device();
        m_renderer = std::make_unique<lumina::graphics::renderer2d>(device);
        m_renderer->init();

        // Create render target for the viewport
        m_render_target = lumina::graphics::render_target::create(
            device, 800, 600, lumina::graphics::format::rgba8_unorm
        );
    }

    void on_detach() override
    {
        m_render_target.reset();
        m_renderer.reset();
    }

    void on_update(float dt) override
    {
        m_time += dt;

        // Camera controls
        float move_speed = 200.0f * dt;
        float zoom_speed = 1.0f * dt;

        if (input::is_key_pressed(input::key_code::w))
            m_camera_pos.y -= move_speed / m_zoom;
        if (input::is_key_pressed(input::key_code::s))
            m_camera_pos.y += move_speed / m_zoom;
        if (input::is_key_pressed(input::key_code::a))
            m_camera_pos.x -= move_speed / m_zoom;
        if (input::is_key_pressed(input::key_code::d))
            m_camera_pos.x += move_speed / m_zoom;

        if (input::is_key_pressed(input::key_code::q))
            m_zoom *= (1.0f + zoom_speed);
        if (input::is_key_pressed(input::key_code::e))
            m_zoom *= (1.0f - zoom_speed);

        m_zoom = glm::clamp(m_zoom, 0.1f, 10.0f);
    }

    void on_render() override
    {
        if (!m_renderer || !m_render_target) return;

        const float width = 800.0f;
        const float height = 600.0f;

        // Create view matrix from camera position and zoom
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(width / 2.0f, height / 2.0f, 0.0f));
        view = glm::scale(view, glm::vec3(m_zoom, m_zoom, 1.0f));
        view = glm::translate(view, glm::vec3(-m_camera_pos.x, -m_camera_pos.y, 0.0f));

        glm::mat4 projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

        m_renderer->begin(view, projection);
        m_renderer->set_render_target(m_render_target);

        // Clear background (large enough to cover when zoomed out)
        m_renderer->draw_quad({
            .position = {m_camera_pos.x, m_camera_pos.y, -0.1f},
            .size = {width * 10, height * 10},
            .color = {0.1f, 0.1f, 0.15f, 1.0f}
        });

        float col_width = width / 4.0f;
        float row_height = height / 2.0f;

        // Row 1: Quads
        m_renderer->draw_quad({
            .position = {col_width * 0.5f, row_height * 0.5f, 0},
            .size = {100, 100},
            .color = {1.0f, 0.3f, 0.3f, 1.0f}
        });

        // Rotated quad
        m_renderer->draw_quad({
            .position = {col_width * 1.5f, row_height * 0.5f, 0},
            .size = {100, 100},
            .color = {0.3f, 1.0f, 0.3f, 1.0f},
            .rotation = m_time
        });

        // Circle (filled)
        m_renderer->draw_circle({
            .position = {col_width * 2.5f, row_height * 0.5f, 0},
            .radius = {50, 50},
            .color = {0.3f, 0.3f, 1.0f, 1.0f}
        });

        // Circle (ring)
        m_renderer->draw_circle({
            .position = {col_width * 3.5f, row_height * 0.5f, 0},
            .radius = {50, 50},
            .color = {1.0f, 1.0f, 0.3f, 1.0f},
            .thickness = 0.1f
        });

        // Row 2: Lines, triangles, rect
        m_renderer->draw_line({
            .start = {col_width * 0.25f, row_height * 1.25f, 0},
            .end = {col_width * 0.75f, row_height * 1.75f, 0},
            .color = {1.0f, 0.5f, 0.0f, 1.0f},
            .thickness = 3.0f
        });

        m_renderer->draw_triangle({
            .p0 = {col_width * 1.5f, row_height * 1.25f, 0},
            .p1 = {col_width * 1.25f, row_height * 1.75f, 0},
            .p2 = {col_width * 1.75f, row_height * 1.75f, 0},
            .color = {0.8f, 0.2f, 0.8f, 1.0f}
        });

        m_renderer->draw_rect({
            .position = {col_width * 2.25f, row_height * 1.25f, 0},
            .size = {col_width * 0.5f, row_height * 0.5f},
            .color = {0.0f, 1.0f, 1.0f, 1.0f},
            .thickness = 2.0f
        });

        // Grid
        m_renderer->draw_grid({
            .position = {col_width * 3.1f, row_height * 1.15f, 0},
            .size = {col_width * 0.8f, row_height * 0.7f},
            .cell_size = 20.0f,
            .line_color = {0.5f, 0.5f, 0.5f, 0.8f},
            .line_width = 1.0f
        });

        m_renderer->end();

        // Display in UI window - Statistics
        ui::begin_window("Statistics");
        ui::text("2D Primitive Rendering Demo");
        ui::separator();

        ui::text("Controls: WASD = Pan, Q/E = Zoom");
        ui::text_fmt("Camera: ({:.0f}, {:.0f})", m_camera_pos.x, m_camera_pos.y);
        ui::text_fmt("Zoom: {:.2f}x", m_zoom);
        ui::separator();

        const auto& stats = m_renderer->get_stats();
        ui::text_fmt("Draw Calls: {}", stats.draw_calls);
        ui::text_fmt("Quads: {}", stats.quad_count);
        ui::text_fmt("Circles: {}", stats.circle_count);
        ui::text_fmt("Lines: {}", stats.line_count);
        ui::text_fmt("Triangles: {}", stats.triangle_count);
        ui::text_fmt("Grids: {}", stats.grid_count);
        m_renderer->reset_stats();
        ui::end_window();

        // Display in UI window - Viewport
        ui::begin_window("Viewport");
        auto tex = m_render_target->get_color_texture();
        if (tex)
        {
            ui::image(tex->get_texture(), ui::get_content_size());
        }
        ui::end_window();
    }

private:
    std::unique_ptr<lumina::graphics::renderer2d> m_renderer;
    lumina::ref<lumina::graphics::render_target> m_render_target;
    float m_time = 0.0f;

    // Camera state
    glm::vec2 m_camera_pos = {400.0f, 300.0f};  // Center of scene
    float m_zoom = 1.0f;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    lumina::core::application_spec spec;
    spec.name = "01-primitives";
    spec.width = 1280;
    spec.height = 720;

    auto* app = new lumina::core::application(spec);
    app->push_layer<primitives_layer>();
    return app;
}
