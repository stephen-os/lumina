// 03-render-target: Render-to-texture
// Demonstrates: Off-screen rendering, sampling render target as texture

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ui = lumina::ui;

class render_target_layer : public lumina::core::layer
{
public:
    render_target_layer() : layer("render_target") {}

    void on_attach() override
    {
        auto& device = lumina::core::application::get().get_device();
        m_renderer = std::make_unique<lumina::graphics::renderer2d>(device);
        m_renderer->init();

        // Create an off-screen render target for the scene
        m_scene_target = lumina::graphics::render_target::create(
            device, 512, 512, lumina::graphics::format::rgba8_unorm
        );

        if (m_scene_target)
        {
            LUMINA_LOG_INFO("Render target created: 512x512");
        }
    }

    void on_detach() override
    {
        m_scene_target.reset();
        m_renderer.reset();
    }

    void on_update(float dt) override
    {
        m_time += dt;
    }

    void on_render() override
    {
        if (!m_renderer || !m_scene_target) return;

        // =====================================================================
        // Render animated scene to off-screen render target
        // =====================================================================
        {
            glm::mat4 rt_projection = glm::ortho(0.0f, 512.0f, 512.0f, 0.0f, -1.0f, 1.0f);

            m_renderer->begin(rt_projection);
            m_renderer->set_render_target(m_scene_target);

            // Clear with dark background
            m_renderer->draw_quad({
                .position = {256, 256, 0},
                .size = {512, 512},
                .color = {0.1f, 0.1f, 0.15f, 1.0f}
            });

            // Animated content
            float cx = 256.0f;
            float cy = 256.0f;

            // Rotating squares
            for (int i = 0; i < 5; i++)
            {
                float angle = m_time + i * (3.14159f * 2.0f / 5.0f);
                float radius = 100.0f;
                float x = cx + std::cos(angle) * radius;
                float y = cy + std::sin(angle) * radius;
                float hue = (float)i / 5.0f;

                m_renderer->draw_quad({
                    .position = {x, y, 0},
                    .size = {40, 40},
                    .color = hsv_to_rgb(hue, 0.8f, 1.0f),
                    .rotation = m_time * 2.0f
                });
            }

            // Center circle
            m_renderer->draw_circle({
                .position = {cx, cy, 0},
                .radius = {30, 30},
                .color = {1, 1, 1, 1}
            });

            // Label
            m_renderer->draw_text({
                .text = "Render Target",
                .position = {256, 480, 0},
                .scale = 1.5f,
                .color = {1, 1, 1, 1},
                .alignment = lumina::graphics::text_alignment::center
            });

            m_renderer->end();
        }

        // =====================================================================
        // Display render targets in UI windows
        // =====================================================================

        // Main viewport
        ui::begin_window("03-render-target");
        ui::text("Render-to-Texture Demo");
        ui::separator();
        ui::text("Render Target: 512x512 RGBA8");
        ui::text("Animated scene rendered to texture");
        ui::separator();

        const auto& stats = m_renderer->get_stats();
        ui::text_fmt("Draw Calls: {}", stats.draw_calls);
        ui::text_fmt("Quads: {}", stats.quad_count);
        m_renderer->reset_stats();

        ui::separator();
        auto tex = m_scene_target->get_color_texture();
        if (tex)
        {
            ui::image(tex->get_texture(), ui::get_content_size());
        }
        ui::end_window();

        // Secondary views showing same texture at different sizes
        ui::begin_window("Multiple Views");
        auto tex2 = m_scene_target->get_color_texture();
        if (tex2)
        {
            ui::text("Small");
            ui::image(tex2->get_texture(), ImVec2(100, 100));

            ui::same_line();

            ui::text("Medium");
            ui::image(tex2->get_texture(), ImVec2(150, 150));

            ui::same_line();

            ui::text("Large");
            ui::image(tex2->get_texture(), ImVec2(200, 200));
        }
        ui::end_window();
    }

private:
    glm::vec4 hsv_to_rgb(float h, float s, float v)
    {
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

    std::unique_ptr<lumina::graphics::renderer2d> m_renderer;
    lumina::ref<lumina::graphics::render_target> m_scene_target;
    float m_time = 0.0f;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    lumina::core::application_spec spec;
    spec.name = "03-render-target";
    spec.width = 1024;
    spec.height = 768;

    auto* app = new lumina::core::application(spec);
    app->push_layer<render_target_layer>();
    return app;
}
