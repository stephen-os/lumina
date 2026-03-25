// 01-primitives: All basic 2D shapes
// Demonstrates: Quad, circle, line, triangle, rectangle outline, pixel

#include <lumina/core/Core.h>
#include <lumina/core/EntryPoint.h>
#include <lumina/graphics/Graphics.h>
#include <lumina/ui/UI.h>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;

class PrimitivesLayer : public Lumina::Layer
{
public:
    PrimitivesLayer() : Layer("Primitives") {}

    void OnAttach() override
    {
        Gfx::Renderer::Init({.Width = 600, .Height = 400});
    }

    void OnDetach() override
    {
        Gfx::Renderer::Shutdown();
    }

    void OnRender() override
    {
        Gfx::Renderer::Begin();
        Gfx::Renderer::Clear({0.1f, 0.1f, 0.12f, 1.0f});

        // Row 1: Quad, Circle (filled), Circle (ring), Ellipse
        Gfx::Renderer::DrawQuad({
            .Position = {100.0f, 100.0f, 0.0f},
            .Size = {80.0f, 80.0f},
            .Color = {1.0f, 0.3f, 0.3f, 1.0f}  // Red
        });

        Gfx::Renderer::DrawCircle({
            .Position = {250.0f, 100.0f, 0.0f},
            .Radius = {40.0f, 40.0f},
            .Color = {0.3f, 1.0f, 0.3f, 1.0f}  // Green
        });

        Gfx::Renderer::DrawCircle({
            .Position = {400.0f, 100.0f, 0.0f},
            .Radius = {40.0f, 40.0f},
            .Color = {0.3f, 0.3f, 1.0f, 1.0f},  // Blue
            .Thickness = 0.15f  // Ring instead of filled
        });

        Gfx::Renderer::DrawCircle({
            .Position = {530.0f, 100.0f, 0.0f},
            .Radius = {50.0f, 30.0f},  // Wider than tall
            .Color = {1.0f, 0.6f, 0.2f, 1.0f}  // Orange
        });

        // Row 2: Line, Triangle, Rectangle outline
        Gfx::Renderer::DrawLine({
            .Start = {60.0f, 250.0f, 0.0f},
            .End = {140.0f, 350.0f, 0.0f},
            .Color = {1.0f, 1.0f, 0.3f, 1.0f},  // Yellow
            .Thickness = 4.0f
        });

        Gfx::Renderer::DrawTriangle({
            .P0 = {250.0f, 250.0f, 0.0f},
            .P1 = {200.0f, 350.0f, 0.0f},
            .P2 = {300.0f, 350.0f, 0.0f},
            .Color = {1.0f, 0.3f, 1.0f, 1.0f}  // Magenta
        });

        Gfx::Renderer::DrawRect({
            .Position = {400.0f, 250.0f, 0.0f},
            .Size = {100.0f, 100.0f},
            .Color = {0.3f, 1.0f, 1.0f, 1.0f},  // Cyan
            .Thickness = 3.0f
        });

        Gfx::Renderer::End();

        // UI
        UI::BeginWindow("Primitives");
        UI::Text("Basic 2D Shapes");
        UI::Separator();
        UI::Text("Row 1: Quad, Circle, Ring, Ellipse");
        UI::Text("Row 2: Line, Triangle, Rectangle");
        UI::Separator();
        auto tex = Gfx::Renderer::GetTexture();
        if (tex)
        {
            UI::Image(tex->GetTexture(), {600, 400});
        }
        UI::EndWindow();
    }
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/01-primitives";
    auto* app = new Application(specs);
    app->PushLayer<PrimitivesLayer>();
    return app;
}
