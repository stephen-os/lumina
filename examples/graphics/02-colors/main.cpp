// 02-colors: Color system demonstration
// Demonstrates: RGBA colors, transparency, alpha blending, color mixing

#include <lumina/core/Core.h>
#include <lumina/core/EntryPoint.h>
#include <lumina/graphics/Graphics.h>
#include <lumina/ui/UI.h>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;

class ColorsLayer : public Lumina::Layer
{
public:
    ColorsLayer() : Layer("Colors") {}

    void OnAttach() override
    {
        Gfx::Renderer::Init({.Width = 600, .Height = 500});
    }

    void OnDetach() override
    {
        Gfx::Renderer::Shutdown();
    }

    void OnRender() override
    {
        Gfx::Renderer::Begin();
        Gfx::Renderer::Clear({0.1f, 0.1f, 0.12f, 1.0f});

        // Row 1: Primary colors
        float y1 = 60.0f;
        Gfx::Renderer::DrawQuad({
            .Position = {100.0f, y1, 0.0f},
            .Size = {80.0f, 80.0f},
            .Color = {1.0f, 0.0f, 0.0f, 1.0f}  // Red
        });
        Gfx::Renderer::DrawQuad({
            .Position = {200.0f, y1, 0.0f},
            .Size = {80.0f, 80.0f},
            .Color = {0.0f, 1.0f, 0.0f, 1.0f}  // Green
        });
        Gfx::Renderer::DrawQuad({
            .Position = {300.0f, y1, 0.0f},
            .Size = {80.0f, 80.0f},
            .Color = {0.0f, 0.0f, 1.0f, 1.0f}  // Blue
        });

        // Row 2: Secondary colors (mixing)
        float y2 = 160.0f;
        Gfx::Renderer::DrawQuad({
            .Position = {100.0f, y2, 0.0f},
            .Size = {80.0f, 80.0f},
            .Color = {1.0f, 1.0f, 0.0f, 1.0f}  // Yellow (R+G)
        });
        Gfx::Renderer::DrawQuad({
            .Position = {200.0f, y2, 0.0f},
            .Size = {80.0f, 80.0f},
            .Color = {0.0f, 1.0f, 1.0f, 1.0f}  // Cyan (G+B)
        });
        Gfx::Renderer::DrawQuad({
            .Position = {300.0f, y2, 0.0f},
            .Size = {80.0f, 80.0f},
            .Color = {1.0f, 0.0f, 1.0f, 1.0f}  // Magenta (R+B)
        });

        // Row 3: Alpha transparency gradient
        float y3 = 260.0f;
        for (int i = 0; i < 5; ++i)
        {
            float alpha = (i + 1) * 0.2f;  // 0.2, 0.4, 0.6, 0.8, 1.0
            Gfx::Renderer::DrawQuad({
                .Position = {80.0f + i * 90.0f, y3, 0.0f},
                .Size = {70.0f, 70.0f},
                .Color = {0.4f, 0.6f, 1.0f, alpha}
            });
        }

        // Row 4: Overlapping transparent circles (alpha blending)
        float y4 = 380.0f;
        Gfx::Renderer::DrawCircle({
            .Position = {150.0f, y4, 0.0f},
            .Radius = {60.0f, 60.0f},
            .Color = {1.0f, 0.2f, 0.2f, 0.6f}  // Semi-transparent red
        });
        Gfx::Renderer::DrawCircle({
            .Position = {200.0f, y4, 0.0f},
            .Radius = {60.0f, 60.0f},
            .Color = {0.2f, 1.0f, 0.2f, 0.6f}  // Semi-transparent green
        });
        Gfx::Renderer::DrawCircle({
            .Position = {175.0f, y4 + 40.0f, 0.0f},
            .Radius = {60.0f, 60.0f},
            .Color = {0.2f, 0.2f, 1.0f, 0.6f}  // Semi-transparent blue
        });

        // Grayscale gradient using circles
        for (int i = 0; i < 6; ++i)
        {
            float gray = i * 0.2f;  // 0.0 to 1.0
            Gfx::Renderer::DrawCircle({
                .Position = {350.0f + i * 40.0f, y4, 0.0f},
                .Radius = {18.0f, 18.0f},
                .Color = {gray, gray, gray, 1.0f}
            });
        }

        Gfx::Renderer::End();

        // UI
        UI::BeginWindow("Colors");
        UI::Text("Color System Demo");
        UI::Separator();
        UI::Text("Row 1: Primary RGB");
        UI::Text("Row 2: Secondary (mixed)");
        UI::Text("Row 3: Alpha transparency");
        UI::Text("Row 4: Blending & grayscale");
        UI::Separator();
        auto tex = Gfx::Renderer::GetTexture();
        if (tex)
        {
            UI::Image(tex->GetTexture(), {600, 500});
        }
        UI::EndWindow();
    }
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/02-colors";
    auto* app = new Application(specs);
    app->PushLayer<ColorsLayer>();
    return app;
}
