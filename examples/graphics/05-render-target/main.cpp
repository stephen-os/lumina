// graphics/05-render-target: Render-to-texture
// Demonstrates: Off-screen rendering, sampling render target as texture

#include <lumina/core/Core.h>
#include <lumina/core/EntryPoint.h>
#include <lumina/graphics/Graphics.h>
#include <lumina/ui/UI.h>

#include <glm/glm.hpp>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;

class RenderTargetLayer : public Lumina::Layer
{
public:
    RenderTargetLayer() : Layer("RenderTarget") {}

    void OnAttach() override
    {
        Gfx::Renderer::Init({.Width = 512, .Height = 512});
    }

    void OnDetach() override
    {
        Gfx::Renderer::Shutdown();
    }

    void OnUpdate(float dt) override
    {
        m_Time += dt;
    }

    void OnRender() override
    {
        float cx = 256.0f;
        float cy = 256.0f;

        Gfx::Renderer::Begin();
        Gfx::Renderer::Clear({0.1f, 0.1f, 0.15f, 1.0f});

        // Rotating squares
        for (int i = 0; i < 5; i++)
        {
            float angle = m_Time + i * (3.14159f * 2.0f / 5.0f);
            float radius = 100.0f;
            float x = cx + std::cos(angle) * radius;
            float y = cy + std::sin(angle) * radius;

            Gfx::Renderer::DrawQuad({
                .Position = {x, y, 0},
                .Size = {40, 40},
                .Color = HsvToRgb(static_cast<float>(i) / 5.0f, 0.8f, 1.0f),
                .Rotation = m_Time * 2.0f
            });
        }

        // Center circle
        Gfx::Renderer::DrawCircle({
            .Position = {cx, cy, 0},
            .Radius = {30, 30},
            .Color = {1, 1, 1, 1}
        });

        // Label
        Gfx::Renderer::DrawText({
            .Text = "Render Target",
            .Position = {256, 480, 0},
            .Scale = 1.5f,
            .Color = {1, 1, 1, 1},
            .Alignment = Gfx::TextAlignment::Center
        });

        Gfx::Renderer::End();

        // Main viewport
        UI::BeginWindow("Render Target Demo");
        UI::Text("Render-to-Texture Demo");
        UI::Separator();
        UI::Text("Render Target: 512x512 RGBA8");
        UI::Text("Animated scene rendered to texture");
        UI::Separator();

        const auto& stats = Gfx::Renderer::GetStats();
        UI::TextFmt("Draw Calls: {}", stats.DrawCalls);
        UI::TextFmt("Quads: {}", stats.QuadCount);
        Gfx::Renderer::ResetStats();

        UI::Separator();
        auto tex = Gfx::Renderer::GetTexture();
        if (tex)
        {
            UI::Image(tex->GetTexture(), UI::GetContentSize());
        }
        UI::EndWindow();

        // Secondary views showing same texture at different sizes
        UI::BeginWindow("Multiple Views");
        if (tex)
        {
            UI::Text("Small");
            UI::Image(tex->GetTexture(), 100, 100);

            UI::SameLine();

            UI::Text("Medium");
            UI::Image(tex->GetTexture(), 150, 150);

            UI::SameLine();

            UI::Text("Large");
            UI::Image(tex->GetTexture(), 200, 200);
        }
        UI::EndWindow();
    }

private:
    glm::vec4 HsvToRgb(float h, float s, float v)
    {
        float c = v * s;
        float x = c * (1 - std::abs(std::fmod(h * 6.0f, 2.0f) - 1));
        float m = v - c;
        float r, g, b;
        if (h < 1.0f/6.0f) { r = c; g = x; b = 0; }
        else if (h < 2.0f/6.0f) { r = x; g = c; b = 0; }
        else if (h < 3.0f/6.0f) { r = 0; g = c; b = x; }
        else if (h < 4.0f/6.0f) { r = 0; g = x; b = c; }
        else if (h < 5.0f/6.0f) { r = x; g = 0; b = c; }
        else { r = c; g = 0; b = x; }
        return {r + m, g + m, b + m, 1.0f};
    }

    float m_Time = 0.0f;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/05-render-target";
    auto* app = new Application(specs);
    app->PushLayer<RenderTargetLayer>();
    return app;
}
