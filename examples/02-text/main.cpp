// 02-text: Text rendering
// Demonstrates: TrueType fonts, scales, colors, alignment

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ui = lumina::ui;

class text_layer : public lumina::core::layer
{
public:
    text_layer() : layer("text") {}

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
        m_frame_time = dt;
    }

    void on_render() override
    {
        if (!m_renderer || !m_render_target) return;

        const float width = 800.0f;
        const float height = 600.0f;

        glm::mat4 projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

        m_renderer->begin(projection);
        m_renderer->set_render_target(m_render_target);

        // Clear background
        m_renderer->draw_quad({
            .position = {width / 2, height / 2, 0},
            .size = {width, height},
            .color = {0.1f, 0.1f, 0.15f, 1.0f}
        });

        float y = 30.0f;

        // Title
        m_renderer->draw_text({
            .text = "TrueType Font Rendering (Embedded Karla)",
            .position = {30, y, 0},
            .scale = 2.0f,
            .color = {1.0f, 0.8f, 0.2f, 1.0f}
        });
        y += 50;

        // Different scales
        m_renderer->draw_text({.text = "Scale 0.5", .position = {30, y, 0}, .scale = 0.5f, .color = {0.7f, 0.7f, 0.7f, 1.0f}});
        y += 15;
        m_renderer->draw_text({.text = "Scale 1.0", .position = {30, y, 0}, .scale = 1.0f, .color = {0.8f, 0.8f, 0.8f, 1.0f}});
        y += 25;
        m_renderer->draw_text({.text = "Scale 1.5", .position = {30, y, 0}, .scale = 1.5f, .color = {0.9f, 0.9f, 0.9f, 1.0f}});
        y += 35;
        m_renderer->draw_text({.text = "Scale 2.0", .position = {30, y, 0}, .scale = 2.0f, .color = {1.0f, 1.0f, 1.0f, 1.0f}});
        y += 50;

        // Colors
        m_renderer->draw_text({.text = "Red", .position = {30, y, 0}, .scale = 2.0f, .color = {1, 0.2f, 0.2f, 1}});
        m_renderer->draw_text({.text = "Green", .position = {120, y, 0}, .scale = 2.0f, .color = {0.2f, 1, 0.2f, 1}});
        m_renderer->draw_text({.text = "Blue", .position = {240, y, 0}, .scale = 2.0f, .color = {0.2f, 0.4f, 1, 1}});
        m_renderer->draw_text({.text = "Yellow", .position = {340, y, 0}, .scale = 2.0f, .color = {1, 1, 0.2f, 1}});
        y += 50;

        // Alignment demo
        float cx = width * 0.5f;
        m_renderer->draw_line({.start = {cx, y, 0}, .end = {cx, y + 100, 0}, .color = {0.5f, 0.5f, 0.5f, 0.5f}});

        m_renderer->draw_text({
            .text = "Left aligned",
            .position = {cx, y, 0},
            .scale = 1.5f,
            .color = {1, 1, 1, 1},
            .alignment = lumina::graphics::text_alignment::left
        });
        y += 30;

        m_renderer->draw_text({
            .text = "Center aligned",
            .position = {cx, y, 0},
            .scale = 1.5f,
            .color = {1, 1, 1, 1},
            .alignment = lumina::graphics::text_alignment::center
        });
        y += 30;

        m_renderer->draw_text({
            .text = "Right aligned",
            .position = {cx, y, 0},
            .scale = 1.5f,
            .color = {1, 1, 1, 1},
            .alignment = lumina::graphics::text_alignment::right
        });
        y += 50;

        // Character sets
        m_renderer->draw_text({.text = "ABCDEFGHIJKLMNOPQRSTUVWXYZ", .position = {30, y, 0}, .scale = 1.5f, .color = {0.8f, 0.8f, 0.8f, 1}});
        y += 30;
        m_renderer->draw_text({.text = "abcdefghijklmnopqrstuvwxyz", .position = {30, y, 0}, .scale = 1.5f, .color = {0.8f, 0.8f, 0.8f, 1}});
        y += 30;
        m_renderer->draw_text({.text = "0123456789", .position = {30, y, 0}, .scale = 1.5f, .color = {0.8f, 0.8f, 0.8f, 1}});
        y += 30;
        m_renderer->draw_text({.text = "!@#$%^&*()_+-=[]{}|;':\",./<>?", .position = {30, y, 0}, .scale = 1.5f, .color = {0.8f, 0.8f, 0.8f, 1}});

        m_renderer->end();

        // Display in UI window
        ui::begin_window("02-text");
        ui::text("Text Rendering Demo");
        ui::separator();

        const auto& stats = m_renderer->get_stats();
        ui::text_fmt("Text Chars: {}", stats.text_char_count);
        ui::text_fmt("Draw Calls: {}", stats.draw_calls);
        ui::text_fmt("FPS: {:.0f}", m_frame_time > 0 ? 1.0f / m_frame_time : 0.0f);
        m_renderer->reset_stats();

        ui::separator();
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
    float m_frame_time = 0.0f;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    lumina::core::application_spec spec;
    spec.name = "02-text";
    spec.width = 1280;
    spec.height = 720;

    auto* app = new lumina::core::application(spec);
    app->push_layer<text_layer>();
    return app;
}
