// ui/00-demo: Lumina UI Demo Window
// Demonstrates: All UI:: functions in one place

#include <Lumina/Core/Core.h>
#include <Lumina/Core/EntryPoint.h>
#include <Lumina/UI/UI.h>

namespace UI = Lumina::UI;

class UIDemoLayer : public Lumina::Layer
{
public:
    UIDemoLayer() : Layer("UIDemoLayer") {}

    void OnRender() override
    {
        // Show the Lumina UI demo window
        UI::ShowDemoWindow(&m_DemoOpen);

        // Also show ImGui's demo for comparison
        if (m_ShowImGuiDemo)
        {
            ImGui::ShowDemoWindow(&m_ShowImGuiDemo);
        }

        // Control window
        UI::BeginWindow("Demo Controls");
        UI::Text("Lumina UI Demo");
        UI::Separator();

        UI::Checkbox("Show Lumina UI Demo", m_DemoOpen);
        UI::Checkbox("Show ImGui Demo (for comparison)", m_ShowImGuiDemo);

        UI::Separator();
        UI::TextDisabled("Press ESC to exit");

        UI::EndWindow();

        // Render notifications (call at end of frame)
        UI::RenderNotifications();
    }

    void OnEvent(Lumina::Event& e) override
    {
        Lumina::EventDispatcher dispatcher(e);

        dispatcher.Dispatch<Lumina::KeyPressedEvent>([](Lumina::KeyPressedEvent& e)
        {
            if (e.GetKey() == Lumina::Input::KeyCode::Escape)
            {
                Lumina::Application::Get().Shutdown();
                return true;
            }
            return false;
        });
    }

private:
    bool m_DemoOpen = true;
    bool m_ShowImGuiDemo = false;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "ui/00-demo";
    auto* app = new Application(specs);
    app->PushLayer<UIDemoLayer>();
    return app;
}
