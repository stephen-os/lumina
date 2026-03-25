// 00-hello-quad: The simplest possible Lumina graphics demo
// Demonstrates: Using the static renderer API to draw a single quad

#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/graphics/graphics.h>
#include <lumina/ui/ui.h>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;

class HelloQuadLayer : public Lumina::Layer
{
public:
    HelloQuadLayer() : Layer("HelloQuad") {}

    void OnAttach() override
    {
        Gfx::Renderer::Init({
            .Width = static_cast<uint32_t>(m_ViewportSize.x),
            .Height = static_cast<uint32_t>(m_ViewportSize.y)
        });
    }

    void OnDetach() override
    {
        Gfx::Renderer::Shutdown();
    }

    void OnRender() override
    {
        // Calculate square size maintaining 1:1 aspect ratio
        float size = std::min(m_ViewportSize.x, m_ViewportSize.y);
        float half = size * 0.5f;

        Gfx::Renderer::Begin();
        Gfx::Renderer::Resize(static_cast<uint32_t>(size), static_cast<uint32_t>(size));
        Gfx::Renderer::Clear({0.1f, 0.1f, 0.12f, 1.0f});

        Gfx::Renderer::DrawQuad({
            .Position = {half, half, 0.0f},
            .Size = {size * 0.375f, size * 0.375f},
            .Color = {0.2f, 0.6f, 1.0f, 1.0f}
        });

        Gfx::Renderer::End();

        // Display in ImGui window
        UI::BeginWindow("Hello Quad");

        m_ViewportSize = UI::GetContentSize();
        auto tex = Gfx::Renderer::GetTexture();
        if (tex)
        {
            UI::Image(tex->GetTexture(), {size, size});
        }
        UI::EndWindow();
    }

private:
    glm::vec2 m_ViewportSize{400.0f, 400.0f};
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/00-hello-quad";
    auto* app = new Application(specs);
    app->PushLayer<HelloQuadLayer>();
    return app;
}
