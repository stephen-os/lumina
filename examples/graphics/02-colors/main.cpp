// 02-colors: Color system demonstration
// Demonstrates: RGBA colors, transparency, alpha blending, color mixing

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ui = lumina::ui;

class colors_layer : public lumina::core::layer
{
public:
    colors_layer() : layer("colors") {}

    void on_attach() override
    {
        auto& device = lumina::core::application::get().get_device();
        m_renderer = std::make_unique<lumina::graphics::renderer2d>(device);
        m_renderer->init();
        m_render_target = lumina::graphics::render_target::create(
            device, 600, 500, lumina::graphics::format::rgba8_unorm
        );
    }

    void on_detach() override
    {
        m_render_target.reset();
        m_renderer.reset();
    }

    void on_render() override
    {
        if (!m_renderer || !m_render_target) return;

        glm::mat4 projection = glm::ortho(0.0f, 600.0f, 500.0f, 0.0f, -1.0f, 1.0f);

        m_renderer->begin(projection);
        m_renderer->set_render_target(m_render_target);
        m_renderer->clear({0.1f, 0.1f, 0.12f, 1.0f});

        // Row 1: Primary colors
        float y1 = 60.0f;
        m_renderer->draw_quad({
            .position = {100.0f, y1, 0.0f},
            .size = {80.0f, 80.0f},
            .color = {1.0f, 0.0f, 0.0f, 1.0f}  // Red
        });
        m_renderer->draw_quad({
            .position = {200.0f, y1, 0.0f},
            .size = {80.0f, 80.0f},
            .color = {0.0f, 1.0f, 0.0f, 1.0f}  // Green
        });
        m_renderer->draw_quad({
            .position = {300.0f, y1, 0.0f},
            .size = {80.0f, 80.0f},
            .color = {0.0f, 0.0f, 1.0f, 1.0f}  // Blue
        });

        // Row 2: Secondary colors (mixing)
        float y2 = 160.0f;
        m_renderer->draw_quad({
            .position = {100.0f, y2, 0.0f},
            .size = {80.0f, 80.0f},
            .color = {1.0f, 1.0f, 0.0f, 1.0f}  // Yellow (R+G)
        });
        m_renderer->draw_quad({
            .position = {200.0f, y2, 0.0f},
            .size = {80.0f, 80.0f},
            .color = {0.0f, 1.0f, 1.0f, 1.0f}  // Cyan (G+B)
        });
        m_renderer->draw_quad({
            .position = {300.0f, y2, 0.0f},
            .size = {80.0f, 80.0f},
            .color = {1.0f, 0.0f, 1.0f, 1.0f}  // Magenta (R+B)
        });

        // Row 3: Alpha transparency gradient
        float y3 = 260.0f;
        for (int i = 0; i < 5; ++i)
        {
            float alpha = (i + 1) * 0.2f;  // 0.2, 0.4, 0.6, 0.8, 1.0
            m_renderer->draw_quad({
                .position = {80.0f + i * 90.0f, y3, 0.0f},
                .size = {70.0f, 70.0f},
                .color = {0.4f, 0.6f, 1.0f, alpha}
            });
        }

        // Row 4: Overlapping transparent circles (alpha blending)
        float y4 = 380.0f;
        m_renderer->draw_circle({
            .position = {150.0f, y4, 0.0f},
            .radius = {60.0f, 60.0f},
            .color = {1.0f, 0.2f, 0.2f, 0.6f}  // Semi-transparent red
        });
        m_renderer->draw_circle({
            .position = {200.0f, y4, 0.0f},
            .radius = {60.0f, 60.0f},
            .color = {0.2f, 1.0f, 0.2f, 0.6f}  // Semi-transparent green
        });
        m_renderer->draw_circle({
            .position = {175.0f, y4 + 40.0f, 0.0f},
            .radius = {60.0f, 60.0f},
            .color = {0.2f, 0.2f, 1.0f, 0.6f}  // Semi-transparent blue
        });

        // Grayscale gradient using circles
        for (int i = 0; i < 6; ++i)
        {
            float gray = i * 0.2f;  // 0.0 to 1.0
            m_renderer->draw_circle({
                .position = {350.0f + i * 40.0f, y4, 0.0f},
                .radius = {18.0f, 18.0f},
                .color = {gray, gray, gray, 1.0f}
            });
        }

        m_renderer->end();

        // UI
        ui::begin_window("Colors");
        ui::text("Color System Demo");
        ui::separator();
        ui::text("Row 1: Primary RGB");
        ui::text("Row 2: Secondary (mixed)");
        ui::text("Row 3: Alpha transparency");
        ui::text("Row 4: Blending & grayscale");
        ui::separator();
        auto tex = m_render_target->get_color_texture();
        if (tex)
        {
            ui::image(tex->get_texture(), {600, 500});
        }
        ui::end_window();
    }

private:
    std::unique_ptr<lumina::graphics::renderer2d> m_renderer;
    lumina::ref<lumina::graphics::render_target> m_render_target;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new lumina::core::application();
    app->set_title("02-colors");
    app->push_layer<colors_layer>();
    return app;
}
