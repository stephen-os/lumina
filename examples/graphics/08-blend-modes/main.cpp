// 10-blend-modes: Blend Modes Demo
// Demonstrates: opaque, alpha, additive, multiply blend modes

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/core/input.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <cmath>

namespace ui = lumina::ui;
namespace gfx = lumina::graphics;
namespace input = lumina::core::input;

class blend_modes_layer : public lumina::core::layer
{
public:
    blend_modes_layer() : layer("blend-modes") {}

    void on_attach() override
    {
        auto& device = lumina::core::application::get().get_device();
        m_renderer = std::make_unique<gfx::renderer2d>(device);
        m_renderer->init();

        // Create render target (square for simplicity)
        m_render_target = gfx::render_target::create(
            device, 600, 600, gfx::format::rgba8_unorm
        );

        // Initialize camera centered at origin
        m_camera = gfx::camera2d(600.0f, 1.0f);
        m_camera.set_position({0.0f, 0.0f});
        m_camera.update(0.0f);
    }

    void on_detach() override
    {
        m_render_target.reset();
        m_renderer.reset();
    }

    void on_update(float dt) override
    {
        m_time += dt;

        // Camera movement (WASD/Arrows)
        float move_speed = 300.0f * dt;
        glm::vec2 move_dir{0.0f};

        if (input::is_key_pressed(input::key_code::w) || input::is_key_pressed(input::key_code::up))
            move_dir.y += 1.0f;
        if (input::is_key_pressed(input::key_code::s) || input::is_key_pressed(input::key_code::down))
            move_dir.y -= 1.0f;
        if (input::is_key_pressed(input::key_code::a) || input::is_key_pressed(input::key_code::left))
            move_dir.x -= 1.0f;
        if (input::is_key_pressed(input::key_code::d) || input::is_key_pressed(input::key_code::right))
            move_dir.x += 1.0f;

        if (glm::length(move_dir) > 0.0f)
        {
            move_dir = glm::normalize(move_dir);
            m_camera.move(move_dir * move_speed / m_camera.get_zoom());
        }

        // Zoom controls (Q/E)
        if (input::is_key_pressed(input::key_code::q))
            m_camera.set_zoom(m_camera.get_zoom() * (1.0f + dt));
        if (input::is_key_pressed(input::key_code::e))
            m_camera.set_zoom(m_camera.get_zoom() * (1.0f - dt));

        m_camera.update(dt);
    }

    void on_render() override
    {
        if (!m_renderer || !m_render_target) return;

        m_renderer->begin(m_camera);
        m_renderer->set_render_target(m_render_target);
        m_renderer->clear({0.0f, 0.0f, 0.0f, 1.0f});  // Clear to black

        // Draw dark background
        m_renderer->draw_quad({
            .position = {0.0f, 0.0f, 0.0f},
            .size = {800.0f, 800.0f},
            .color = m_bg_color
        });

        // Draw a simple grid of colored squares
        const int grid_size = 5;
        const float quad_size = 80.0f;
        const float spacing = 100.0f;
        const float start = -(grid_size - 1) * spacing / 2.0f;

        for (int row = 0; row < grid_size; row++)
        {
            for (int col = 0; col < grid_size; col++)
            {
                float x = start + col * spacing;
                float y = start + row * spacing;

                glm::vec4 color = ((row + col) % 2 == 0) ? m_grid_color1 : m_grid_color2;

                m_renderer->draw_quad({
                    .position = {x, y, 0.1f},
                    .size = {quad_size, quad_size},
                    .color = color
                });
            }
        }

        // Center quad with selectable blend mode
        gfx::blend_mode blend = gfx::blend_mode::alpha;
        switch (m_center_blend_mode)
        {
            case 0: blend = gfx::blend_mode::opaque; break;
            case 1: blend = gfx::blend_mode::alpha; break;
            case 2: blend = gfx::blend_mode::additive; break;
            case 3: blend = gfx::blend_mode::multiply; break;
        }

        m_renderer->draw_quad({
            .position = {0.0f, 0.0f, 0.2f},
            .size = {50.0f, 50.0f},
            .color = m_center_color,
            .blend = blend
        });

        // Scissor test: draw quads clipped to a region
        if (m_scissor_enabled)
        {
            // Push scissor region (centered, 200x200 pixels)
            float scissor_size = 200.0f;
            float scissor_x = (600.0f - scissor_size) / 2.0f;
            float scissor_y = (600.0f - scissor_size) / 2.0f;
            m_renderer->push_scissor(scissor_x, scissor_y, scissor_size, scissor_size);

            // Draw some quads that should be clipped
            for (int i = 0; i < 4; i++)
            {
                float angle = m_time + i * 1.57f;
                float x = std::cos(angle) * 150.0f;
                float y = std::sin(angle) * 150.0f;

                m_renderer->draw_quad({
                    .position = {x, y, 0.5f},
                    .size = {80.0f, 80.0f},
                    .color = {0.0f, 0.5f, 1.0f, 0.8f},
                    .blend = gfx::blend_mode::alpha
                });
            }

            m_renderer->pop_scissor();

            // Draw outline bars around the scissor region (in world space, follows camera)
            // Convert scissor screen coords to world coords relative to camera
            glm::vec2 cam_pos = m_camera.get_position();
            float half_size = scissor_size / 2.0f / m_camera.get_zoom();
            float bar_thickness = 4.0f;
            glm::vec4 bar_color = {1.0f, 0.0f, 1.0f, 1.0f};  // Magenta

            // Top bar
            m_renderer->draw_quad({
                .position = {cam_pos.x, cam_pos.y + half_size, 0.9f},
                .size = {scissor_size / m_camera.get_zoom() + bar_thickness, bar_thickness},
                .color = bar_color
            });
            // Bottom bar
            m_renderer->draw_quad({
                .position = {cam_pos.x, cam_pos.y - half_size, 0.9f},
                .size = {scissor_size / m_camera.get_zoom() + bar_thickness, bar_thickness},
                .color = bar_color
            });
            // Left bar
            m_renderer->draw_quad({
                .position = {cam_pos.x - half_size, cam_pos.y, 0.9f},
                .size = {bar_thickness, scissor_size / m_camera.get_zoom() + bar_thickness},
                .color = bar_color
            });
            // Right bar
            m_renderer->draw_quad({
                .position = {cam_pos.x + half_size, cam_pos.y, 0.9f},
                .size = {bar_thickness, scissor_size / m_camera.get_zoom() + bar_thickness},
                .color = bar_color
            });
        }

        m_renderer->end();

        // UI
        render_ui();
    }

    void render_ui()
    {
        ui::begin_window("Blend Modes Demo");

        ui::text("Blend Mode Debug Test");
        ui::separator();

        // Color controls
        ui::text("Colors:");
        ImGui::ColorEdit4("Background", &m_bg_color.x);
        ImGui::ColorEdit4("Grid Color 1", &m_grid_color1.x);
        ImGui::ColorEdit4("Grid Color 2", &m_grid_color2.x);
        ImGui::ColorEdit4("Center Quad", &m_center_color.x);
        ui::separator();

        // Blend mode selection
        ui::text("Center Quad Blend Mode:");
        ImGui::RadioButton("Opaque", &m_center_blend_mode, 0);
        ImGui::RadioButton("Alpha", &m_center_blend_mode, 1);
        ImGui::RadioButton("Additive", &m_center_blend_mode, 2);
        ImGui::RadioButton("Multiply", &m_center_blend_mode, 3);
        ui::separator();

        // Scissor test
        ui::text("Scissor Clipping:");
        ImGui::Checkbox("Enable Scissor Test", &m_scissor_enabled);
        if (m_scissor_enabled)
        {
            ui::text("Blue quads orbit and are clipped to center 200x200 region");
        }
        ui::separator();

        ui::text("Controls: WASD/Arrows=Pan, Q/E=Zoom");
        ui::separator();

        auto pos = m_camera.get_position();
        ui::text_fmt("Camera: ({:.0f}, {:.0f}) Zoom: {:.2f}x", pos.x, pos.y, m_camera.get_zoom());
        ui::separator();

        const auto& stats = m_renderer->get_stats();
        ui::text_fmt("Draw Calls: {}", stats.draw_calls);
        ui::text_fmt("Quads: {}", stats.quad_count);
        m_renderer->reset_stats();

        ui::end_window();

        // Viewport
        ui::push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ui::begin_window("Viewport");

        auto content_size = ui::get_content_size();
        if (auto tex = m_render_target->get_color_texture())
        {
            ui::image(tex->get_texture(), content_size);
        }

        ui::end_window();
        ui::pop_style_var();
    }

private:
    lumina::scope<gfx::renderer2d> m_renderer;
    lumina::ref<gfx::render_target> m_render_target;
    gfx::camera2d m_camera;

    float m_time = 0.0f;

    // Color controls for debugging
    glm::vec4 m_bg_color{0.1f, 0.1f, 0.15f, 1.0f};
    glm::vec4 m_grid_color1{0.8f, 0.3f, 0.3f, 1.0f};  // Red
    glm::vec4 m_grid_color2{0.3f, 0.8f, 0.3f, 1.0f};  // Green
    glm::vec4 m_center_color{1.0f, 1.0f, 0.0f, 0.7f}; // Yellow
    int m_center_blend_mode = 2;  // 0=opaque, 1=alpha, 2=additive, 3=multiply
    bool m_scissor_enabled = false;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new application(graphics_api::vulkan);
    app->set_title("graphics/08-blend-modes");
    app->push_layer<blend_modes_layer>();
    return app;
}
