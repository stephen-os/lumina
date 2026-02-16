// 01-primitives: All basic 2D shapes
// Demonstrates: Quad, circle, line, triangle, rectangle outline, pixel

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ui = lumina::ui;

class primitives_layer : public lumina::core::layer
{
public:
    primitives_layer() : layer("primitives") {}

    void on_attach() override
    {
        auto& device = lumina::core::application::get().get_device();
        m_renderer = std::make_unique<lumina::graphics::renderer2d>(device);
        m_renderer->init();
        m_render_target = lumina::graphics::render_target::create(
            device, 600, 400, lumina::graphics::format::rgba8_unorm
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

        glm::mat4 projection = glm::ortho(0.0f, 600.0f, 400.0f, 0.0f, -1.0f, 1.0f);

        m_renderer->begin(projection);
        m_renderer->set_render_target(m_render_target);
        m_renderer->clear({0.1f, 0.1f, 0.12f, 1.0f});

        // Row 1: Quad, Circle (filled), Circle (ring)
        m_renderer->draw_quad({
            .position = {100.0f, 100.0f, 0.0f},
            .size = {80.0f, 80.0f},
            .color = {1.0f, 0.3f, 0.3f, 1.0f}  // Red
        });

        m_renderer->draw_circle({
            .position = {250.0f, 100.0f, 0.0f},
            .radius = {40.0f, 40.0f},
            .color = {0.3f, 1.0f, 0.3f, 1.0f}  // Green
        });

        m_renderer->draw_circle({
            .position = {400.0f, 100.0f, 0.0f},
            .radius = {40.0f, 40.0f},
            .color = {0.3f, 0.3f, 1.0f, 1.0f},  // Blue
            .thickness = 0.15f  // Ring instead of filled
        });

        // Row 2: Line, Triangle, Rectangle outline
        m_renderer->draw_line({
            .start = {60.0f, 250.0f, 0.0f},
            .end = {140.0f, 350.0f, 0.0f},
            .color = {1.0f, 1.0f, 0.3f, 1.0f},  // Yellow
            .thickness = 4.0f
        });

        m_renderer->draw_triangle({
            .p0 = {250.0f, 250.0f, 0.0f},
            .p1 = {200.0f, 350.0f, 0.0f},
            .p2 = {300.0f, 350.0f, 0.0f},
            .color = {1.0f, 0.3f, 1.0f, 1.0f}  // Magenta
        });

        m_renderer->draw_rect({
            .position = {400.0f, 250.0f, 0.0f},
            .size = {100.0f, 100.0f},
            .color = {0.3f, 1.0f, 1.0f, 1.0f},  // Cyan
            .thickness = 3.0f
        });

        // Bonus: Ellipse (circle with different x/y radii)
        m_renderer->draw_circle({
            .position = {530.0f, 100.0f, 0.0f},
            .radius = {50.0f, 30.0f},  // Wider than tall
            .color = {1.0f, 0.6f, 0.2f, 1.0f}  // Orange
        });

        m_renderer->end();

        // UI
        ui::begin_window("Primitives");
        ui::text("Basic 2D Shapes");
        ui::separator();
        ui::text("Row 1: Quad, Circle, Ring, Ellipse");
        ui::text("Row 2: Line, Triangle, Rectangle");
        ui::separator();
        auto tex = m_render_target->get_color_texture();
        if (tex)
        {
            ui::image(tex->get_texture(), {600, 400});
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
    app->set_title("01-primitives");
    app->push_layer<primitives_layer>();
    return app;
}
