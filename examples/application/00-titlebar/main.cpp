// application/00-titlebar: Custom window titlebar colors
// Demonstrates: Windows 11+ titlebar color customization

#include <lumina/core/Core.h>
#include <lumina/core/EntryPoint.h>
#include <lumina/ui/UI.h>

namespace UI = Lumina::UI;

class TitlebarLayer : public Lumina::Layer
{
public:
    TitlebarLayer() : Layer("Titlebar") {}

    void OnRender() override
    {
        UI::BeginWindow("Titlebar Demo");
        UI::Text("Custom titlebar colors!");
        UI::Spacing();
        UI::Text("Titlebar: Dark Gray (30, 30, 30)");
        UI::Text("Text: Lumina Orange (255, 128, 0)");
        UI::Spacing();
        UI::TextDisabled("Note: Requires Windows 11+");
        UI::EndWindow();
    }
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "application/00-titlebar";
    specs.Titlebar = ApplicationSpecifications::TitlebarTheme{
        .Background = {30.0f / 255.0f, 30.0f / 255.0f, 30.0f / 255.0f},
        .Text = {1.0f, 128.0f / 255.0f, 0.0f}
    };
    auto* app = new Application(specs);
    app->PushLayer<TitlebarLayer>();
    return app;
}
