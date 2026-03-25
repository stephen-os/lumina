// 08-blend-modes: Blend Modes Demo
// Demonstrates: Opaque, alpha, additive, multiply blend modes with scissor clipping

#include <lumina/core/Core.h>
#include <lumina/core/EntryPoint.h>
#include <lumina/core/Input.h>
#include <lumina/graphics/Graphics.h>
#include <lumina/ui/UI.h>

#include <glm/glm.hpp>
#include <cmath>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;
namespace Input = Lumina::Input;

class BlendModesLayer : public Lumina::Layer
{
public:
    BlendModesLayer() : Layer("blend-modes") {}

    void OnAttach() override
    {
        auto& device = Lumina::Application::Get().GetDevice();
        m_Renderer = std::make_unique<Gfx::Renderer2D>(device);
        m_Renderer->Init();

        // Create render target (square for simplicity)
        m_RenderTarget = Gfx::RenderTarget::Create(
            device, 600, 600, Gfx::Format::RGBA8Unorm
        );

        // Initialize camera centered at origin
        m_Camera = Gfx::Camera2D(600.0f, 1.0f);
        m_Camera.SetPosition({0.0f, 0.0f});
        m_Camera.Update(0.0f);
    }

    void OnDetach() override
    {
        m_RenderTarget.reset();
        m_Renderer.reset();
    }

    void OnUpdate(float dt) override
    {
        m_Time += dt;

        // Camera movement (WASD/Arrows)
        float moveSpeed = 300.0f * dt;
        glm::vec2 moveDir{0.0f};

        if (Input::IsKeyPressed(Input::KeyCode::W) || Input::IsKeyPressed(Input::KeyCode::Up))
            moveDir.y += 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::S) || Input::IsKeyPressed(Input::KeyCode::Down))
            moveDir.y -= 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::A) || Input::IsKeyPressed(Input::KeyCode::Left))
            moveDir.x -= 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::D) || Input::IsKeyPressed(Input::KeyCode::Right))
            moveDir.x += 1.0f;

        if (glm::length(moveDir) > 0.0f)
        {
            moveDir = glm::normalize(moveDir);
            m_Camera.Move(moveDir * moveSpeed / m_Camera.GetZoom());
        }

        // Zoom controls (Q/E)
        if (Input::IsKeyPressed(Input::KeyCode::Q))
            m_Camera.SetZoom(m_Camera.GetZoom() * (1.0f + dt));
        if (Input::IsKeyPressed(Input::KeyCode::E))
            m_Camera.SetZoom(m_Camera.GetZoom() * (1.0f - dt));

        m_Camera.Update(dt);
    }

    void OnRender() override
    {
        if (!m_Renderer || !m_RenderTarget) return;

        m_Renderer->Begin(m_Camera);
        m_Renderer->SetRenderTarget(m_RenderTarget);
        m_Renderer->Clear({0.0f, 0.0f, 0.0f, 1.0f});  // Clear to black

        // Draw dark background
        m_Renderer->DrawQuad({
            .Position = {0.0f, 0.0f, 0.0f},
            .Size = {800.0f, 800.0f},
            .Color = m_BgColor
        });

        // Draw a simple grid of colored squares
        const int gridSize = 5;
        const float quadSize = 80.0f;
        const float spacing = 100.0f;
        const float start = -(gridSize - 1) * spacing / 2.0f;

        for (int row = 0; row < gridSize; row++)
        {
            for (int col = 0; col < gridSize; col++)
            {
                float x = start + col * spacing;
                float y = start + row * spacing;

                glm::vec4 color = ((row + col) % 2 == 0) ? m_GridColor1 : m_GridColor2;

                m_Renderer->DrawQuad({
                    .Position = {x, y, 0.1f},
                    .Size = {quadSize, quadSize},
                    .Color = color
                });
            }
        }

        // Center quad with selectable blend mode
        Gfx::BlendMode blend = Gfx::BlendMode::Alpha;
        switch (m_CenterBlendMode)
        {
            case 0: blend = Gfx::BlendMode::Opaque; break;
            case 1: blend = Gfx::BlendMode::Alpha; break;
            case 2: blend = Gfx::BlendMode::Additive; break;
            case 3: blend = Gfx::BlendMode::Multiply; break;
        }

        m_Renderer->DrawQuad({
            .Position = {0.0f, 0.0f, 0.2f},
            .Size = {50.0f, 50.0f},
            .Color = m_CenterColor,
            .Blend = blend
        });

        // Scissor test: draw quads clipped to a region
        if (m_ScissorEnabled)
        {
            // Push scissor region (centered, 200x200 pixels)
            float scissorSize = 200.0f;
            float scissorX = (600.0f - scissorSize) / 2.0f;
            float scissorY = (600.0f - scissorSize) / 2.0f;
            m_Renderer->PushScissor(scissorX, scissorY, scissorSize, scissorSize);

            // Draw some quads that should be clipped
            for (int i = 0; i < 4; i++)
            {
                float angle = m_Time + i * 1.57f;
                float x = std::cos(angle) * 150.0f;
                float y = std::sin(angle) * 150.0f;

                m_Renderer->DrawQuad({
                    .Position = {x, y, 0.5f},
                    .Size = {80.0f, 80.0f},
                    .Color = {0.0f, 0.5f, 1.0f, 0.8f},
                    .Blend = Gfx::BlendMode::Alpha
                });
            }

            m_Renderer->PopScissor();

            // Draw outline bars around the scissor region (in world space, follows camera)
            // Convert scissor screen coords to world coords relative to camera
            glm::vec2 camPos = m_Camera.GetPosition();
            float halfSize = scissorSize / 2.0f / m_Camera.GetZoom();
            float barThickness = 4.0f;
            glm::vec4 barColor = {1.0f, 0.0f, 1.0f, 1.0f};  // Magenta

            // Top bar
            m_Renderer->DrawQuad({
                .Position = {camPos.x, camPos.y + halfSize, 0.9f},
                .Size = {scissorSize / m_Camera.GetZoom() + barThickness, barThickness},
                .Color = barColor
            });
            // Bottom bar
            m_Renderer->DrawQuad({
                .Position = {camPos.x, camPos.y - halfSize, 0.9f},
                .Size = {scissorSize / m_Camera.GetZoom() + barThickness, barThickness},
                .Color = barColor
            });
            // Left bar
            m_Renderer->DrawQuad({
                .Position = {camPos.x - halfSize, camPos.y, 0.9f},
                .Size = {barThickness, scissorSize / m_Camera.GetZoom() + barThickness},
                .Color = barColor
            });
            // Right bar
            m_Renderer->DrawQuad({
                .Position = {camPos.x + halfSize, camPos.y, 0.9f},
                .Size = {barThickness, scissorSize / m_Camera.GetZoom() + barThickness},
                .Color = barColor
            });
        }

        m_Renderer->End();

        // UI
        RenderUI();
    }

    void RenderUI()
    {
        UI::BeginWindow("Blend Modes Demo");

        UI::Text("Blend Mode Debug Test");
        UI::Separator();

        // Color controls
        UI::Text("Colors:");
        ImGui::ColorEdit4("Background", &m_BgColor.x);
        ImGui::ColorEdit4("Grid Color 1", &m_GridColor1.x);
        ImGui::ColorEdit4("Grid Color 2", &m_GridColor2.x);
        ImGui::ColorEdit4("Center Quad", &m_CenterColor.x);
        UI::Separator();

        // Blend mode selection
        UI::Text("Center Quad Blend Mode:");
        ImGui::RadioButton("Opaque", &m_CenterBlendMode, 0);
        ImGui::RadioButton("Alpha", &m_CenterBlendMode, 1);
        ImGui::RadioButton("Additive", &m_CenterBlendMode, 2);
        ImGui::RadioButton("Multiply", &m_CenterBlendMode, 3);
        UI::Separator();

        // Scissor test
        UI::Text("Scissor Clipping:");
        ImGui::Checkbox("Enable Scissor Test", &m_ScissorEnabled);
        if (m_ScissorEnabled)
        {
            UI::Text("Blue quads orbit and are clipped to center 200x200 region");
        }
        UI::Separator();

        UI::Text("Controls: WASD/Arrows=Pan, Q/E=Zoom");
        UI::Separator();

        auto pos = m_Camera.GetPosition();
        UI::TextFmt("Camera: ({:.0f}, {:.0f}) Zoom: {:.2f}x", pos.x, pos.y, m_Camera.GetZoom());
        UI::Separator();

        const auto& stats = m_Renderer->GetStats();
        UI::TextFmt("Draw Calls: {}", stats.DrawCalls);
        UI::TextFmt("Quads: {}", stats.QuadCount);
        m_Renderer->ResetStats();

        UI::EndWindow();

        // Viewport
        UI::PushStyleVar(ImGuiStyleVar_WindowPadding, glm::vec2(0, 0));
        UI::BeginWindow("Viewport");

        auto contentSize = UI::GetContentSize();
        if (auto tex = m_RenderTarget->GetColorTexture())
        {
            UI::Image(tex->GetTexture(), contentSize);
        }

        UI::EndWindow();
        UI::PopStyleVar();
    }

private:
    Lumina::Scope<Gfx::Renderer2D> m_Renderer;
    Lumina::Ref<Gfx::RenderTarget> m_RenderTarget;
    Gfx::Camera2D m_Camera;

    float m_Time = 0.0f;

    // Color controls for debugging
    glm::vec4 m_BgColor{0.1f, 0.1f, 0.15f, 1.0f};
    glm::vec4 m_GridColor1{0.8f, 0.3f, 0.3f, 1.0f};  // Red
    glm::vec4 m_GridColor2{0.3f, 0.8f, 0.3f, 1.0f};  // Green
    glm::vec4 m_CenterColor{1.0f, 1.0f, 0.0f, 0.7f}; // Yellow
    int m_CenterBlendMode = 2;  // 0=opaque, 1=alpha, 2=additive, 3=multiply
    bool m_ScissorEnabled = false;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/08-blend-modes";
    auto* app = new Application(specs);
    app->PushLayer<BlendModesLayer>();
    return app;
}
