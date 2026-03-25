// graphics/04-text: Text rendering
// Demonstrates: TrueType fonts, scales, colors, alignment

#include <Lumina/Core/Core.h>
#include <Lumina/Core/EntryPoint.h>
#include <Lumina/Graphics/Graphics.h>
#include <Lumina/UI/UI.h>

#include <glm/glm.hpp>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;

class TextLayer : public Lumina::Layer
{
public:
    TextLayer() : Layer("Text") {}

    void OnAttach() override
    {
        Gfx::Renderer::Init({.Width = 800, .Height = 600});
    }

    void OnDetach() override
    {
        Gfx::Renderer::Shutdown();
    }

    void OnUpdate(float dt) override
    {
        m_FrameTime = dt;
    }

    void OnRender() override
    {
        const float width = 800.0f;
        const float height = 600.0f;

        Gfx::Renderer::Begin();
        Gfx::Renderer::Clear({0.1f, 0.1f, 0.15f, 1.0f});

        float y = 30.0f;

        // Title
        Gfx::Renderer::DrawText({
            .Text = "TrueType Font Rendering (Embedded Karla)",
            .Position = {30, y, 0},
            .Scale = 2.0f,
            .Color = {1.0f, 0.8f, 0.2f, 1.0f}
        });
        y += 50;

        // Different scales
        Gfx::Renderer::DrawText({.Text = "Scale 0.5", .Position = {30, y, 0}, .Scale = 0.5f, .Color = {0.7f, 0.7f, 0.7f, 1.0f}});
        y += 15;
        Gfx::Renderer::DrawText({.Text = "Scale 1.0", .Position = {30, y, 0}, .Scale = 1.0f, .Color = {0.8f, 0.8f, 0.8f, 1.0f}});
        y += 25;
        Gfx::Renderer::DrawText({.Text = "Scale 1.5", .Position = {30, y, 0}, .Scale = 1.5f, .Color = {0.9f, 0.9f, 0.9f, 1.0f}});
        y += 35;
        Gfx::Renderer::DrawText({.Text = "Scale 2.0", .Position = {30, y, 0}, .Scale = 2.0f, .Color = {1.0f, 1.0f, 1.0f, 1.0f}});
        y += 50;

        // Colors
        Gfx::Renderer::DrawText({.Text = "Red", .Position = {30, y, 0}, .Scale = 2.0f, .Color = {1, 0.2f, 0.2f, 1}});
        Gfx::Renderer::DrawText({.Text = "Green", .Position = {120, y, 0}, .Scale = 2.0f, .Color = {0.2f, 1, 0.2f, 1}});
        Gfx::Renderer::DrawText({.Text = "Blue", .Position = {240, y, 0}, .Scale = 2.0f, .Color = {0.2f, 0.4f, 1, 1}});
        Gfx::Renderer::DrawText({.Text = "Yellow", .Position = {340, y, 0}, .Scale = 2.0f, .Color = {1, 1, 0.2f, 1}});
        y += 50;

        // Alignment demo
        float cx = width * 0.5f;
        Gfx::Renderer::DrawLine({.Start = {cx, y, 0}, .End = {cx, y + 100, 0}, .Color = {0.5f, 0.5f, 0.5f, 0.5f}});

        Gfx::Renderer::DrawText({
            .Text = "Left aligned",
            .Position = {cx, y, 0},
            .Scale = 1.5f,
            .Color = {1, 1, 1, 1},
            .Alignment = Gfx::TextAlignment::Left
        });
        y += 30;

        Gfx::Renderer::DrawText({
            .Text = "Center aligned",
            .Position = {cx, y, 0},
            .Scale = 1.5f,
            .Color = {1, 1, 1, 1},
            .Alignment = Gfx::TextAlignment::Center
        });
        y += 30;

        Gfx::Renderer::DrawText({
            .Text = "Right aligned",
            .Position = {cx, y, 0},
            .Scale = 1.5f,
            .Color = {1, 1, 1, 1},
            .Alignment = Gfx::TextAlignment::Right
        });
        y += 50;

        // Character sets
        Gfx::Renderer::DrawText({.Text = "ABCDEFGHIJKLMNOPQRSTUVWXYZ", .Position = {30, y, 0}, .Scale = 1.5f, .Color = {0.8f, 0.8f, 0.8f, 1}});
        y += 30;
        Gfx::Renderer::DrawText({.Text = "abcdefghijklmnopqrstuvwxyz", .Position = {30, y, 0}, .Scale = 1.5f, .Color = {0.8f, 0.8f, 0.8f, 1}});
        y += 30;
        Gfx::Renderer::DrawText({.Text = "0123456789", .Position = {30, y, 0}, .Scale = 1.5f, .Color = {0.8f, 0.8f, 0.8f, 1}});
        y += 30;
        Gfx::Renderer::DrawText({.Text = "!@#$%^&*()_+-=[]{}|;':\",./<>?", .Position = {30, y, 0}, .Scale = 1.5f, .Color = {0.8f, 0.8f, 0.8f, 1}});

        Gfx::Renderer::End();

        // Display in UI window
        UI::BeginWindow("Text Demo");
        UI::Text("Text Rendering Demo");
        UI::Separator();

        const auto& stats = Gfx::Renderer::GetStats();
        UI::TextFmt("Text Chars: {}", stats.TextCharCount);
        UI::TextFmt("Draw Calls: {}", stats.DrawCalls);
        UI::TextFmt("FPS: {:.0f}", m_FrameTime > 0 ? 1.0f / m_FrameTime : 0.0f);
        Gfx::Renderer::ResetStats();

        UI::Separator();
        auto tex = Gfx::Renderer::GetTexture();
        if (tex)
        {
            UI::Image(tex->GetTexture(), UI::GetContentSize());
        }
        UI::EndWindow();
    }

private:
    float m_FrameTime = 0.0f;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/04-text";
    auto* app = new Application(specs);
    app->PushLayer<TextLayer>();
    return app;
}
