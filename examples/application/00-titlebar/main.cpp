// application/00-titlebar: Custom window titlebar colors
// Demonstrates: Windows 11+ titlebar color customization

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/ui/ui.h>

namespace ui = lumina::ui;

class titlebar_layer : public lumina::core::layer
{
public:
    titlebar_layer() : layer("titlebar") {}

    void on_render() override
    {
        ui::begin_window("Titlebar Demo");
        ui::text("Custom titlebar colors!");
        ui::spacing();
        ui::text("Titlebar: Dark Gray (30, 30, 30)");
        ui::text("Text: Lumina Orange (255, 128, 0)");
        ui::spacing();
        ui::text_disabled("Note: Requires Windows 11+");
        ui::end_window();
    }
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    auto* app = new lumina::core::application();
    app->set_title("application/00-titlebar");
    app->set_titlebar_color(30, 30, 30);
    app->set_titlebar_text_color(255, 128, 0);
    app->push_layer<titlebar_layer>();
    return app;
}
