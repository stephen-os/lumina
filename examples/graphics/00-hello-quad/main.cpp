// 00-hello-quad: The simplest possible Lumina graphics demo
// Demonstrates: Using the static renderer API to draw a single quad

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

namespace ui = lumina::ui;
namespace gfx = lumina::graphics;

class hello_quad_layer : public lumina::core::layer
{
public:
    hello_quad_layer() : layer("hello_quad") {}

    void on_attach() override
    {
        gfx::renderer::init(m_viewport_size);
    }

    void on_detach() override
    {
        gfx::renderer::shutdown();
    }

    void on_render() override
    {
        // Calculate square size maintaining 1:1 aspect ratio
        float size = std::min(m_viewport_size.x, m_viewport_size.y);
        float half = size * 0.5f;

        gfx::renderer::begin();
        gfx::renderer::resize(static_cast<uint32_t>(size), static_cast<uint32_t>(size));
        gfx::renderer::clear({0.1f, 0.1f, 0.12f, 1.0f});

        gfx::renderer::draw_quad({
            .position = {half, half, 0.0f},
            .size = {size * 0.375f, size * 0.375f},
            .color = {0.2f, 0.6f, 1.0f, 1.0f}
        });

        gfx::renderer::end();

        // Display in ImGui window
        ui::begin_window("Hello Quad");

        m_viewport_size = ui::get_content_size();
        auto tex = gfx::renderer::get_texture();
        if (tex)
        {
            ui::image(tex->get_texture(), {size, size});
        }
        ui::end_window();
    }

private:
    glm::vec2 m_viewport_size{400.0f, 400.0f};
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new lumina::core::application();
    app->set_title("00-hello-quad");
    app->push_layer<hello_quad_layer>();
    return app;
}
