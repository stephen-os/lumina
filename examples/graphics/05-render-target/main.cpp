// graphics/05-render-target: Render-to-texture
// Demonstrates: Off-screen rendering, sampling render target as texture

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>

namespace ui = lumina::ui;
namespace gfx = lumina::graphics;

class render_target_layer : public lumina::core::layer
{
public:
    render_target_layer() : layer("render_target") {}

    void on_attach() override
    {
        gfx::renderer::init({.width = 512, .height = 512});
    }

    void on_detach() override
    {
        gfx::renderer::shutdown();
    }

    void on_update(float dt) override
    {
        m_time += dt;
    }

    void on_render() override
    {
        float cx = 256.0f;
        float cy = 256.0f;

        gfx::renderer::begin();
        gfx::renderer::clear({0.1f, 0.1f, 0.15f, 1.0f});

        // Rotating squares
        for (int i = 0; i < 5; i++)
        {
            float angle = m_time + i * (3.14159f * 2.0f / 5.0f);
            float radius = 100.0f;
            float x = cx + std::cos(angle) * radius;
            float y = cy + std::sin(angle) * radius;

            gfx::renderer::draw_quad({
                .position = {x, y, 0},
                .size = {40, 40},
                .color = hsv_to_rgb(static_cast<float>(i) / 5.0f, 0.8f, 1.0f),
                .rotation = m_time * 2.0f
            });
        }

        // Center circle
        gfx::renderer::draw_circle({
            .position = {cx, cy, 0},
            .radius = {30, 30},
            .color = {1, 1, 1, 1}
        });

        // Label
        gfx::renderer::draw_text({
            .text = "Render Target",
            .position = {256, 480, 0},
            .scale = 1.5f,
            .color = {1, 1, 1, 1},
            .alignment = gfx::text_alignment::center
        });

        gfx::renderer::end();

        // Main viewport
        ui::begin_window("Render Target Demo");
        ui::text("Render-to-Texture Demo");
        ui::separator();
        ui::text("Render Target: 512x512 RGBA8");
        ui::text("Animated scene rendered to texture");
        ui::separator();

        const auto& stats = gfx::renderer::get_stats();
        ui::text_fmt("Draw Calls: {}", stats.draw_calls);
        ui::text_fmt("Quads: {}", stats.quad_count);
        gfx::renderer::reset_stats();

        ui::separator();
        auto tex = gfx::renderer::get_texture();
        if (tex)
        {
            ui::image(tex->get_texture(), ui::get_content_size());
        }
        ui::end_window();

        // Secondary views showing same texture at different sizes
        ui::begin_window("Multiple Views");
        if (tex)
        {
            ui::text("Small");
            ui::image(tex->get_texture(), 100, 100);

            ui::same_line();

            ui::text("Medium");
            ui::image(tex->get_texture(), 150, 150);

            ui::same_line();

            ui::text("Large");
            ui::image(tex->get_texture(), 200, 200);
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

    float m_time = 0.0f;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    application_specifications specs;
    specs.title = "graphics/05-render-target";
    auto* app = new application(specs);
    app->push_layer<render_target_layer>();
    return app;
}
