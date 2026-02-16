// 01-primitives: All basic 2D shapes
// Demonstrates: Quad, circle, line, triangle, rectangle outline, pixel

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

namespace ui = lumina::ui;
namespace gfx = lumina::graphics;

class primitives_layer : public lumina::core::layer
{
public:
    primitives_layer() : layer("primitives") {}

    void on_attach() override
    {
        gfx::renderer::init(600, 400);
    }

    void on_detach() override
    {
        gfx::renderer::shutdown();
    }

    void on_render() override
    {
        gfx::renderer::begin();
        gfx::renderer::clear({0.1f, 0.1f, 0.12f, 1.0f});

        // Row 1: Quad, Circle (filled), Circle (ring), Ellipse
        gfx::renderer::draw_quad({
            .position = {100.0f, 100.0f, 0.0f},
            .size = {80.0f, 80.0f},
            .color = {1.0f, 0.3f, 0.3f, 1.0f}  // Red
        });

        gfx::renderer::draw_circle({
            .position = {250.0f, 100.0f, 0.0f},
            .radius = {40.0f, 40.0f},
            .color = {0.3f, 1.0f, 0.3f, 1.0f}  // Green
        });

        gfx::renderer::draw_circle({
            .position = {400.0f, 100.0f, 0.0f},
            .radius = {40.0f, 40.0f},
            .color = {0.3f, 0.3f, 1.0f, 1.0f},  // Blue
            .thickness = 0.15f  // Ring instead of filled
        });

        gfx::renderer::draw_circle({
            .position = {530.0f, 100.0f, 0.0f},
            .radius = {50.0f, 30.0f},  // Wider than tall
            .color = {1.0f, 0.6f, 0.2f, 1.0f}  // Orange
        });

        // Row 2: Line, Triangle, Rectangle outline
        gfx::renderer::draw_line({
            .start = {60.0f, 250.0f, 0.0f},
            .end = {140.0f, 350.0f, 0.0f},
            .color = {1.0f, 1.0f, 0.3f, 1.0f},  // Yellow
            .thickness = 4.0f
        });

        gfx::renderer::draw_triangle({
            .p0 = {250.0f, 250.0f, 0.0f},
            .p1 = {200.0f, 350.0f, 0.0f},
            .p2 = {300.0f, 350.0f, 0.0f},
            .color = {1.0f, 0.3f, 1.0f, 1.0f}  // Magenta
        });

        gfx::renderer::draw_rect({
            .position = {400.0f, 250.0f, 0.0f},
            .size = {100.0f, 100.0f},
            .color = {0.3f, 1.0f, 1.0f, 1.0f},  // Cyan
            .thickness = 3.0f
        });

        gfx::renderer::end();

        // UI
        ui::begin_window("Primitives");
        ui::text("Basic 2D Shapes");
        ui::separator();
        ui::text("Row 1: Quad, Circle, Ring, Ellipse");
        ui::text("Row 2: Line, Triangle, Rectangle");
        ui::separator();
        auto tex = gfx::renderer::get_texture();
        if (tex)
        {
            ui::image(tex->get_texture(), {600, 400});
        }
        ui::end_window();
    }
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new lumina::core::application();
    app->set_title("graphics/01-primitives");
    app->push_layer<primitives_layer>();
    return app;
}
