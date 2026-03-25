// Lumina Sandbox - Starter Project

#include <Lumina/Core/Core.h>
#include <Lumina/Core/EntryPoint.h>
#include <Lumina/UI/UI.h>

namespace UI = Lumina::UI;

class SandboxLayer : public Lumina::Layer
{
public:
    SandboxLayer() : Layer("Sandbox") {}

    void OnAttach() override
    {
        LUMINA_LOG_INFO("Sandbox layer attached");
    }

    void OnDetach() override
    {
        LUMINA_LOG_INFO("Sandbox layer detached");
    }

    void OnUpdate(float dt) override
    {
        m_FrameTime = dt;
    }

    void OnRender() override
    {
        UI::BeginWindow("Sandbox");
        UI::Text("Welcome to Lumina!");
        UI::Separator();
        UI::TextFmt("Frame Time: {:.3f} ms", m_FrameTime * 1000.0f);
        UI::TextFmt("FPS: {:.1f}", m_FrameTime > 0.0f ? 1.0f / m_FrameTime : 0.0f);
        UI::EndWindow();
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
    float m_FrameTime = 0.0f;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    Lumina::ApplicationSpecifications specs;
    specs.Title = "Sandbox";
    auto* app = new Lumina::Application(specs);
    app->PushLayer<SandboxLayer>();
    return app;
}
