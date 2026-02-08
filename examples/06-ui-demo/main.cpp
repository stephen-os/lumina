// 06-ui-demo: Lumina UI Demo Window
// Demonstrates all ui:: functions in one place

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/ui/ui.h>

namespace ui = lumina::ui;

class ui_demo_layer : public lumina::core::layer
{
public:
    ui_demo_layer() : layer("ui_demo") {}

    void on_attach() override
    {
        LUMINA_LOG_INFO("UI Demo layer attached");
    }

    void on_render() override
    {
        // Show the Lumina UI demo window
        ui::show_demo_window(&m_demo_open);

        // Also show ImGui's demo for comparison
        if (m_show_imgui_demo)
        {
            ImGui::ShowDemoWindow(&m_show_imgui_demo);
        }

        // Control window
        ui::begin_window("Demo Controls");
        ui::text("Lumina UI Demo");
        ui::separator();

        ui::checkbox("Show Lumina UI Demo", m_demo_open);
        ui::checkbox("Show ImGui Demo (for comparison)", m_show_imgui_demo);

        ui::separator();
        ui::text_disabled("Press ESC to exit");

        ui::end_window();

        // Render notifications (call at end of frame)
        ui::render_notifications();
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
    bool m_demo_open = true;
    bool m_show_imgui_demo = false;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    lumina::core::application_spec spec;
    spec.name = "06-ui-demo";
    spec.width = 1400;
    spec.height = 900;

    auto* app = new lumina::core::application(spec);
    app->push_layer<ui_demo_layer>();
    return app;
}
