// 07-lighting: 2D Lighting Demo
// Demonstrates: Point lights with various attenuation models, ambient light

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/core/input.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ui = lumina::ui;
namespace input = lumina::core::input;
namespace gfx = lumina::graphics;

class lighting_layer : public lumina::core::layer
{
public:
    lighting_layer() : layer("lighting") {}

    void on_attach() override
    {
        auto& device = lumina::core::application::get().get_device();
        m_renderer = std::make_unique<gfx::renderer2d>(device);
        m_renderer->init();

        // Create initial render target (will be resized to match viewport)
        m_render_target = gfx::render_target::create(
            device, 800, 600, gfx::format::rgba8_unorm
        );

        LUMINA_LOG_INFO("Lighting demo attached");
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
        if (input::is_key_pressed(input::key_code::w))
            m_camera_pos.y -= move_speed / m_zoom;
        if (input::is_key_pressed(input::key_code::s))
            m_camera_pos.y += move_speed / m_zoom;
        if (input::is_key_pressed(input::key_code::a))
            m_camera_pos.x -= move_speed / m_zoom;
        if (input::is_key_pressed(input::key_code::d))
            m_camera_pos.x += move_speed / m_zoom;

        // Animate mouse light position towards cursor (when in viewport)
        if (m_follow_mouse)
        {
            m_mouse_light_pos = m_mouse_world_pos;
        }
    }

    void on_render() override
    {
        if (!m_renderer) return;

        // UI Controls window
        render_controls_ui();

        // Viewport window - handles render target sizing, rendering, and display
        render_viewport();
    }

    void render_scene(float width, float height)
    {
        // Create view matrix from camera position and zoom
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(width / 2.0f, height / 2.0f, 0.0f));
        view = glm::scale(view, glm::vec3(m_zoom, m_zoom, 1.0f));
        view = glm::translate(view, glm::vec3(-m_camera_pos.x, -m_camera_pos.y, 0.0f));

        glm::mat4 projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

        // Enable lighting
        m_renderer->set_lighting_enabled(m_lighting_enabled);
        m_renderer->set_ambient_light(m_ambient_color, m_ambient_intensity);

        m_renderer->begin(view, projection);
        m_renderer->set_render_target(m_render_target);

        // Draw background - extends beyond viewport for camera panning
        m_renderer->draw_quad({
            .position = {m_camera_pos.x - width * 2, m_camera_pos.y - height * 2, -0.1f},
            .size = {width * 5, height * 5},
            .color = {0.15f, 0.15f, 0.2f, 1.0f}
        });

        // Draw a grid pattern to show lighting better
        for (int y = 0; y < 12; y++)
        {
            for (int x = 0; x < 20; x++)
            {
                float px = 40.0f + x * 80.0f;
                float py = 40.0f + y * 80.0f;

                glm::vec4 color = ((x + y) % 2 == 0)
                    ? glm::vec4(0.4f, 0.4f, 0.5f, 1.0f)
                    : glm::vec4(0.3f, 0.3f, 0.4f, 1.0f);

                m_renderer->draw_quad({
                    .position = {px, py, 0},
                    .size = {70, 70},
                    .color = color
                });
            }
        }

        // Draw some colored objects
        m_renderer->draw_circle({
            .position = {300, 200, 0},
            .radius = {50, 50},
            .color = {0.8f, 0.2f, 0.2f, 1.0f}
        });

        m_renderer->draw_circle({
            .position = {width / 2, height / 2, 0},
            .radius = {70, 70},
            .color = {0.2f, 0.8f, 0.2f, 1.0f}
        });

        m_renderer->draw_quad({
            .position = {width - 330, height - 200, 0},
            .size = {120, 100},
            .color = {0.2f, 0.2f, 0.8f, 1.0f},
            .rotation = m_time * 0.5f
        });

        m_renderer->draw_triangle({
            .p0 = {450, 480, 0},
            .p1 = {380, 600, 0},
            .p2 = {520, 600, 0},
            .color = {0.8f, 0.8f, 0.2f, 1.0f}
        });

        // Add point lights
        if (m_lighting_enabled)
        {
            float center_x = width / 2;
            float center_y = height / 2;

            // Orbiting red light
            float orbit_x = center_x + std::cos(m_time) * 250;
            float orbit_y = center_y + std::sin(m_time) * 180;
            m_renderer->draw_point_light({
                .position = {orbit_x, orbit_y, 0},
                .color = {1.0f, 0.3f, 0.3f},
                .intensity = m_light_intensity,
                .radius = m_light_radius,
                .attenuation = m_attenuation_model
            });

            // Orbiting blue light (opposite phase)
            float orbit_x2 = center_x + std::cos(m_time + 3.14159f) * 250;
            float orbit_y2 = center_y + std::sin(m_time + 3.14159f) * 180;
            m_renderer->draw_point_light({
                .position = {orbit_x2, orbit_y2, 0},
                .color = {0.3f, 0.3f, 1.0f},
                .intensity = m_light_intensity,
                .radius = m_light_radius,
                .attenuation = m_attenuation_model
            });

            // Static green light at center
            m_renderer->draw_point_light({
                .position = {center_x, center_y, 0},
                .color = {0.3f, 1.0f, 0.3f},
                .intensity = m_light_intensity * 0.5f,
                .radius = m_light_radius * 1.5f,
                .attenuation = m_attenuation_model
            });

            // Mouse-following light
            if (m_show_mouse_light)
            {
                m_renderer->draw_point_light({
                    .position = {m_mouse_light_pos.x, m_mouse_light_pos.y, 0},
                    .color = m_mouse_light_color,
                    .intensity = m_light_intensity,
                    .radius = m_light_radius,
                    .attenuation = m_attenuation_model
                });
            }
        }

        m_renderer->end();
    }

    void render_controls_ui()
    {
        ui::begin_window("Lighting Controls");
        ui::text("2D Lighting Demo");
        ui::separator();

        ui::text("Controls: WASD = Pan");
        ui::text_fmt("Camera: ({:.0f}, {:.0f})", m_camera_pos.x, m_camera_pos.y);
        ui::text_fmt("Viewport: {}x{}", m_viewport_width, m_viewport_height);
        ui::separator();

        ui::checkbox("Enable Lighting", m_lighting_enabled);
        ui::separator();

        if (m_lighting_enabled)
        {
            ui::text("Ambient Light");
            ui::property_color3("Ambient Color", glm::value_ptr(m_ambient_color));
            ui::property_slider("Ambient Intensity", m_ambient_intensity, 0.0f, 2.0f);
            ui::separator();

            ui::text("Point Lights");
            ui::property_slider("Radius", m_light_radius, 50.0f, 500.0f);
            ui::property_slider("Light Intensity", m_light_intensity, 0.1f, 5.0f);

            // Attenuation model dropdown
            static const char* attenuation_names[] = {
                "None", "Linear", "Quadratic", "Inverse Square",
                "Exponential", "Smoothstep", "Realistic"
            };
            int current = static_cast<int>(m_attenuation_model);
            if (ui::property_dropdown("Attenuation", current, attenuation_names, 7))
            {
                m_attenuation_model = static_cast<gfx::attenuation_model>(current);
            }
            ui::separator();

            ui::text("Mouse Light");
            ui::checkbox("Show Mouse Light", m_show_mouse_light);
            ui::checkbox("Follow Mouse", m_follow_mouse);
            if (m_show_mouse_light)
            {
                ui::property_color3("Mouse Color", glm::value_ptr(m_mouse_light_color));
            }
        }
        ui::separator();

        const auto& stats = m_renderer->get_stats();
        ui::text_fmt("Draw Calls: {}", stats.draw_calls);
        ui::text_fmt("Point Lights: {}", stats.point_light_count);
        m_renderer->reset_stats();

        ui::end_window();
    }

    void render_viewport()
    {
        // Viewport window - remove padding for clean image display
        ui::push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ui::begin_window("Viewport");

        // Get available content size
        auto content_size = ui::get_content_size();
        uint32_t new_width = static_cast<uint32_t>(std::max(1.0f, content_size.x));
        uint32_t new_height = static_cast<uint32_t>(std::max(1.0f, content_size.y));

        // Resize render target if viewport size changed
        if (new_width != m_viewport_width || new_height != m_viewport_height)
        {
            m_viewport_width = new_width;
            m_viewport_height = new_height;

            if (m_render_target)
            {
                m_render_target->resize(m_viewport_width, m_viewport_height);
            }
        }

        // Render the scene at viewport size
        if (m_render_target && m_viewport_width > 0 && m_viewport_height > 0)
        {
            render_scene(static_cast<float>(m_viewport_width), static_cast<float>(m_viewport_height));
        }

        // Display at 1:1 scale
        auto tex = m_render_target ? m_render_target->get_color_texture() : nullptr;
        ui::image_rect img_rect = {};
        if (tex)
        {
            img_rect = ui::image_with_rect(tex->get_texture(), content_size);
        }

        // Calculate mouse position in world coordinates
        auto mouse_pos = ui::get_mouse_pos();
        if (ui::is_item_hovered())
        {
            // At 1:1 scale, local position IS render target position
            float rt_x = mouse_pos.x - img_rect.pos.x;
            float rt_y = mouse_pos.y - img_rect.pos.y;

            // Convert to world coordinates considering camera
            float half_width = static_cast<float>(m_viewport_width) / 2.0f;
            float half_height = static_cast<float>(m_viewport_height) / 2.0f;
            m_mouse_world_pos.x = m_camera_pos.x + (rt_x - half_width) / m_zoom;
            m_mouse_world_pos.y = m_camera_pos.y + (rt_y - half_height) / m_zoom;
        }

        ui::end_window();
        ui::pop_style_var();
    }

private:
    std::unique_ptr<gfx::renderer2d> m_renderer;
    lumina::ref<gfx::render_target> m_render_target;
    float m_time = 0.0f;

    // Viewport state
    uint32_t m_viewport_width = 0;
    uint32_t m_viewport_height = 0;

    // Camera state
    glm::vec2 m_camera_pos = {400.0f, 300.0f};
    float m_zoom = 1.0f;

    // Lighting settings
    bool m_lighting_enabled = true;
    glm::vec3 m_ambient_color = {0.1f, 0.1f, 0.15f};
    float m_ambient_intensity = 1.0f;
    float m_light_radius = 200.0f;
    float m_light_intensity = 1.5f;
    gfx::attenuation_model m_attenuation_model = gfx::attenuation_model::quadratic;

    // Mouse light
    bool m_show_mouse_light = true;
    bool m_follow_mouse = true;
    glm::vec3 m_mouse_light_color = {1.0f, 0.9f, 0.7f};
    glm::vec2 m_mouse_light_pos = {400, 300};
    glm::vec2 m_mouse_world_pos = {400, 300};
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new lumina::core::application();
    app->set_title("graphics/06-lighting");
    app->push_layer<lighting_layer>();
    return app;
}
