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
    application_specifications specs;
    specs.title = "application/00-titlebar";
    specs.titlebar = application_specifications::titlebar_theme{
        .background = {30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f},
        .text = {1.0f, 128.0f / 255.0f, 0.0f}
    };
    auto* app = new application(specs);
    app->push_layer<titlebar_layer>();
    return app;
}
