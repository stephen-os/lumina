// Lumina Sandbox - Starter Project

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/ui/ui.h>

namespace ui = lumina::ui;

class sandbox_layer : public lumina::core::layer
{
public:
    sandbox_layer() : layer("sandbox") {}

    void on_attach() override
    {
        LUMINA_LOG_INFO("Sandbox layer attached");
    }

    void on_detach() override
    {
        LUMINA_LOG_INFO("Sandbox layer detached");
    }

    void on_update(float dt) override
    {
        m_frame_time = dt;
    }

    void on_render() override
    {
        ui::begin_window("Sandbox");
        ui::text("Welcome to Lumina!");
        ui::separator();
        ui::text_fmt("Frame Time: {:.3f} ms", m_frame_time * 1000.0f);
        ui::text_fmt("FPS: {:.1f}", m_frame_time > 0.0f ? 1.0f / m_frame_time : 0.0f);
        ui::end_window();
    }

    void on_event(lumina::core::event& e) override
    {
        lumina::core::event_dispatcher dispatcher(e);

        dispatcher.dispatch<lumina::core::key_pressed_event>([](lumina::core::key_pressed_event& e)
        {
            if (e.get_key() == lumina::core::input::key_code::escape)
            {
                lumina::core::application::get().shutdown();
                return true;
            }
            return false;
        });
    }

private:
    float m_frame_time = 0.0f;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    lumina::core::application_specifications specs;
    specs.title = "Sandbox";
    auto* app = new lumina::core::application(specs);
    app->push_layer<sandbox_layer>();
    return app;
}
