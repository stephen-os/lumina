// 03-transforms: Transformation demonstration
// Demonstrates: Rotation, scaling, rotation origin points

#include <lumina/core/Core.h>
#include <lumina/core/EntryPoint.h>
#include <lumina/graphics/Graphics.h>
#include <lumina/ui/UI.h>

#include <glm/glm.hpp>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;

class TransformsLayer : public Lumina::Layer
{
public:
    TransformsLayer() : Layer("Transforms") {}

    void OnAttach() override
    {
        Gfx::Renderer::Init({.Width = 600, .Height = 500});
    }

    void OnDetach() override
    {
        Gfx::Renderer::Shutdown();
    }

    void OnUpdate(float deltaTime) override
    {
        m_Time += deltaTime;
    }

    void OnRender() override
    {
        Gfx::Renderer::Begin();
        Gfx::Renderer::Clear({0.1f, 0.1f, 0.12f, 1.0f});

        // Row 1: Static rotations (0, 15, 30, 45, 60, 90 degrees)
        float y1 = 80.0f;
        const float angles[] = {0.0f, 15.0f, 30.0f, 45.0f, 60.0f, 90.0f};
        for (int i = 0; i < 6; ++i)
        {
            Gfx::Renderer::DrawQuad({
                .Position = {80.0f + i * 90.0f, y1, 0.0f},
                .Size = {50.0f, 30.0f},
                .Color = {0.3f, 0.6f, 1.0f, 1.0f},
                .Rotation = glm::radians(angles[i])
            });
        }

        // Row 2: Different sizes (scaling demonstration)
        float y2 = 180.0f;
        const float sizes[] = {20.0f, 35.0f, 50.0f, 65.0f, 80.0f};
        for (int i = 0; i < 5; ++i)
        {
            Gfx::Renderer::DrawQuad({
                .Position = {100.0f + i * 100.0f, y2, 0.0f},
                .Size = {sizes[i], sizes[i]},
                .Color = {1.0f, 0.5f, 0.2f, 1.0f}
            });
        }

        // Row 3: Animated rotation (spinning shapes)
        float y3 = 300.0f;
        float spin = m_Time * 2.0f;  // 2 radians per second

        // Slow spin
        Gfx::Renderer::DrawQuad({
            .Position = {100.0f, y3, 0.0f},
            .Size = {60.0f, 40.0f},
            .Color = {1.0f, 0.3f, 0.3f, 1.0f},
            .Rotation = spin * 0.5f
        });

        // Medium spin
        Gfx::Renderer::DrawQuad({
            .Position = {220.0f, y3, 0.0f},
            .Size = {60.0f, 40.0f},
            .Color = {0.3f, 1.0f, 0.3f, 1.0f},
            .Rotation = spin
        });

        // Fast spin
        Gfx::Renderer::DrawQuad({
            .Position = {340.0f, y3, 0.0f},
            .Size = {60.0f, 40.0f},
            .Color = {0.3f, 0.3f, 1.0f, 1.0f},
            .Rotation = spin * 2.0f
        });

        // Counter-rotation pair
        Gfx::Renderer::DrawQuad({
            .Position = {480.0f, y3, 0.0f},
            .Size = {70.0f, 70.0f},
            .Color = {1.0f, 0.8f, 0.2f, 0.7f},
            .Rotation = spin
        });
        Gfx::Renderer::DrawQuad({
            .Position = {480.0f, y3, 0.0f},
            .Size = {45.0f, 45.0f},
            .Color = {0.8f, 0.2f, 1.0f, 0.8f},
            .Rotation = -spin * 1.5f
        });

        // Row 4: Pulsing scale animation
        float y4 = 420.0f;
        float pulse = std::sin(m_Time * 3.0f) * 0.3f + 1.0f;  // Scale 0.7 to 1.3

        Gfx::Renderer::DrawCircle({
            .Position = {150.0f, y4, 0.0f},
            .Radius = {30.0f * pulse, 30.0f * pulse},
            .Color = {0.4f, 1.0f, 0.8f, 1.0f}
        });

        // Breathing rectangle
        float breathe = std::sin(m_Time * 2.0f) * 0.2f + 1.0f;
        Gfx::Renderer::DrawQuad({
            .Position = {300.0f, y4, 0.0f},
            .Size = {80.0f * breathe, 50.0f * breathe},
            .Color = {1.0f, 0.6f, 0.4f, 1.0f}
        });

        // Ellipse with changing aspect ratio
        float aspect = std::sin(m_Time * 1.5f) * 0.5f + 1.0f;
        Gfx::Renderer::DrawCircle({
            .Position = {450.0f, y4, 0.0f},
            .Radius = {40.0f * aspect, 40.0f / aspect},
            .Color = {0.6f, 0.4f, 1.0f, 1.0f}
        });

        Gfx::Renderer::End();

        // UI
        UI::BeginWindow("Transforms");
        UI::Text("Transform Animations");
        UI::Separator();
        UI::Text("Row 1: Static rotations");
        UI::Text("Row 2: Size variations");
        UI::Text("Row 3: Animated rotation");
        UI::Text("Row 4: Scale animations");
        UI::Separator();
        auto tex = Gfx::Renderer::GetTexture();
        if (tex)
        {
            UI::Image(tex->GetTexture(), {600, 500});
        }
        UI::EndWindow();
    }

private:
    float m_Time = 0.0f;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/03-transforms";
    auto* app = new Application(specs);
    app->PushLayer<TransformsLayer>();
    return app;
}
