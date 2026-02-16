// 03-transforms: Transformation demonstration
// Demonstrates: Rotation, scaling, rotation origin points

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ui = lumina::ui;

class transforms_layer : public lumina::core::layer
{
public:
    transforms_layer() : layer("transforms") {}

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

    void on_update(float delta_time) override
    {
        m_time += delta_time;
    }

    void on_render() override
    {
        if (!m_renderer || !m_render_target) return;

        glm::mat4 projection = glm::ortho(0.0f, 600.0f, 500.0f, 0.0f, -1.0f, 1.0f);

        m_renderer->begin(projection);
        m_renderer->set_render_target(m_render_target);
        m_renderer->clear({0.1f, 0.1f, 0.12f, 1.0f});

        // Row 1: Static rotations (0, 15, 30, 45, 60, 90 degrees)
        float y1 = 80.0f;
        const float angles[] = {0.0f, 15.0f, 30.0f, 45.0f, 60.0f, 90.0f};
        for (int i = 0; i < 6; ++i)
        {
            m_renderer->draw_quad({
                .position = {80.0f + i * 90.0f, y1, 0.0f},
                .size = {50.0f, 30.0f},
                .color = {0.3f, 0.6f, 1.0f, 1.0f},
                .rotation = glm::radians(angles[i])
            });
        }

        // Row 2: Different sizes (scaling demonstration)
        float y2 = 180.0f;
        const float sizes[] = {20.0f, 35.0f, 50.0f, 65.0f, 80.0f};
        for (int i = 0; i < 5; ++i)
        {
            m_renderer->draw_quad({
                .position = {100.0f + i * 100.0f, y2, 0.0f},
                .size = {sizes[i], sizes[i]},
                .color = {1.0f, 0.5f, 0.2f, 1.0f}
            });
        }

        // Row 3: Animated rotation (spinning shapes)
        float y3 = 300.0f;
        float spin = m_time * 2.0f;  // 2 radians per second

        // Slow spin
        m_renderer->draw_quad({
            .position = {100.0f, y3, 0.0f},
            .size = {60.0f, 40.0f},
            .color = {1.0f, 0.3f, 0.3f, 1.0f},
            .rotation = spin * 0.5f
        });

        // Medium spin
        m_renderer->draw_quad({
            .position = {220.0f, y3, 0.0f},
            .size = {60.0f, 40.0f},
            .color = {0.3f, 1.0f, 0.3f, 1.0f},
            .rotation = spin
        });

        // Fast spin
        m_renderer->draw_quad({
            .position = {340.0f, y3, 0.0f},
            .size = {60.0f, 40.0f},
            .color = {0.3f, 0.3f, 1.0f, 1.0f},
            .rotation = spin * 2.0f
        });

        // Counter-rotation pair
        m_renderer->draw_quad({
            .position = {480.0f, y3, 0.0f},
            .size = {70.0f, 70.0f},
            .color = {1.0f, 0.8f, 0.2f, 0.7f},
            .rotation = spin
        });
        m_renderer->draw_quad({
            .position = {480.0f, y3, 0.0f},
            .size = {45.0f, 45.0f},
            .color = {0.8f, 0.2f, 1.0f, 0.8f},
            .rotation = -spin * 1.5f
        });

        // Row 4: Pulsing scale animation
        float y4 = 420.0f;
        float pulse = std::sin(m_time * 3.0f) * 0.3f + 1.0f;  // Scale 0.7 to 1.3

        m_renderer->draw_circle({
            .position = {150.0f, y4, 0.0f},
            .radius = {30.0f * pulse, 30.0f * pulse},
            .color = {0.4f, 1.0f, 0.8f, 1.0f}
        });

        // Breathing rectangle
        float breathe = std::sin(m_time * 2.0f) * 0.2f + 1.0f;
        m_renderer->draw_quad({
            .position = {300.0f, y4, 0.0f},
            .size = {80.0f * breathe, 50.0f * breathe},
            .color = {1.0f, 0.6f, 0.4f, 1.0f}
        });

        // Ellipse with changing aspect ratio
        float aspect = std::sin(m_time * 1.5f) * 0.5f + 1.0f;
        m_renderer->draw_circle({
            .position = {450.0f, y4, 0.0f},
            .radius = {40.0f * aspect, 40.0f / aspect},
            .color = {0.6f, 0.4f, 1.0f, 1.0f}
        });

        m_renderer->end();

        // UI
        ui::begin_window("Transforms");
        ui::text("Transform Animations");
        ui::separator();
        ui::text("Row 1: Static rotations");
        ui::text("Row 2: Size variations");
        ui::text("Row 3: Animated rotation");
        ui::text("Row 4: Scale animations");
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
    float m_time = 0.0f;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new lumina::core::application();
    app->set_title("03-transforms");
    app->push_layer<transforms_layer>();
    return app;
}
