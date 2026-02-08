// 00-basic: Minimal Lumina application
// Demonstrates: Render-to-texture, displaying rendered content in ImGui window

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ui = lumina::ui;

class basic_layer : public lumina::core::layer
{
public:
    basic_layer() : layer("basic") {}

    void on_attach() override
    {
        auto& device = lumina::core::application::get().get_device();
        m_renderer = std::make_unique<lumina::graphics::renderer2d>(device);
        m_renderer->init();

        // Create render target for the viewport
        m_render_target = lumina::graphics::render_target::create(
            device, 512, 512, lumina::graphics::format::rgba8_unorm
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

        // Render scene to texture
        glm::mat4 projection = glm::ortho(0.0f, 512.0f, 512.0f, 0.0f, -1.0f, 1.0f);

        m_renderer->begin(projection);
        m_renderer->set_render_target(m_render_target);

        // Clear background
        m_renderer->draw_quad({
            .position = {256, 256, 0},
            .size = {512, 512},
            .color = {0.1f, 0.1f, 0.15f, 1.0f}
        });

        // Draw a centered colored quad
        m_renderer->draw_quad({
            .position = {256, 256, 0},
            .size = {200, 200},
            .color = {0.2f, 0.6f, 1.0f, 1.0f}
        });

        m_renderer->end();

        // Display in UI window
        ui::begin_window("00-basic");
        ui::text("Minimal Lumina application");
        ui::text("Rendered to 512x512 texture");
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
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    lumina::core::application_spec spec;
    spec.name = "00-basic";
    spec.width = 1280;
    spec.height = 720;

    auto* app = new lumina::core::application(spec);
    app->push_layer<basic_layer>();
    return app;
}
