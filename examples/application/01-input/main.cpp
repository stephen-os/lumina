// application/01-input: Input System Demo
// Demonstrates: Keyboard polling, mouse position, mouse buttons, visual feedback

#include <lumina/core/Core.h>
#include <lumina/core/EntryPoint.h>
#include <lumina/core/Input.h>
#include <lumina/graphics/Graphics.h>
#include <lumina/ui/UI.h>

#include <glm/glm.hpp>
#include <algorithm>

namespace UI = Lumina::UI;
namespace Gfx = Lumina::Graphics;
namespace Input = Lumina::Input;

class InputLayer : public Lumina::Layer
{
public:
    InputLayer() : Layer("Input") {}

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
        // Move player with WASD or arrow keys
        float speed = 200.0f * dt;
        glm::vec2 moveDir{0.0f};

        if (Input::IsKeyPressed(Input::KeyCode::W) || Input::IsKeyPressed(Input::KeyCode::Up))
            moveDir.y -= 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::S) || Input::IsKeyPressed(Input::KeyCode::Down))
            moveDir.y += 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::A) || Input::IsKeyPressed(Input::KeyCode::Left))
            moveDir.x -= 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::D) || Input::IsKeyPressed(Input::KeyCode::Right))
            moveDir.x += 1.0f;

        // Normalize diagonal movement
        if (glm::length(moveDir) > 0.0f)
        {
            moveDir = glm::normalize(moveDir);
            m_PlayerPos += moveDir * speed;
        }

        // Clamp to bounds
        m_PlayerPos.x = std::clamp(m_PlayerPos.x, 25.0f, 487.0f);
        m_PlayerPos.y = std::clamp(m_PlayerPos.y, 25.0f, 487.0f);

        // Track modifier keys
        m_ShiftHeld = Input::IsKeyPressed(Input::KeyCode::LeftShift) ||
                      Input::IsKeyPressed(Input::KeyCode::RightShift);
        m_CtrlHeld = Input::IsKeyPressed(Input::KeyCode::LeftControl) ||
                     Input::IsKeyPressed(Input::KeyCode::RightControl);
        m_AltHeld = Input::IsKeyPressed(Input::KeyCode::LeftAlt) ||
                    Input::IsKeyPressed(Input::KeyCode::RightAlt);

        // Track mouse buttons
        m_MouseLeft = Input::IsMouseButtonPressed(Input::MouseCode::Left);
        m_MouseRight = Input::IsMouseButtonPressed(Input::MouseCode::Right);
        m_MouseMiddle = Input::IsMouseButtonPressed(Input::MouseCode::Middle);

        // Get mouse position
        auto mouse = Input::GetMousePosition();
        m_MousePos = {mouse.x, mouse.y};
    }

    void OnRender() override
    {
        Gfx::Renderer::Begin();
        Gfx::Renderer::Clear({0.12f, 0.12f, 0.15f, 1.0f});

        // Draw grid for reference
        for (int i = 0; i <= 8; i++)
        {
            float pos = i * 64.0f;
            Gfx::Renderer::DrawLine({
                .Start = {pos, 0, 0},
                .End = {pos, 512, 0},
                .Color = {0.2f, 0.2f, 0.25f, 1.0f}
            });
            Gfx::Renderer::DrawLine({
                .Start = {0, pos, 0},
                .End = {512, pos, 0},
                .Color = {0.2f, 0.2f, 0.25f, 1.0f}
            });
        }

        // Draw player square (changes color based on modifiers)
        glm::vec4 playerColor = {0.2f, 0.6f, 1.0f, 1.0f};  // Blue
        if (m_ShiftHeld) playerColor = {1.0f, 0.6f, 0.2f, 1.0f};  // Orange
        if (m_CtrlHeld) playerColor = {0.2f, 1.0f, 0.6f, 1.0f};   // Green
        if (m_AltHeld) playerColor = {1.0f, 0.2f, 0.6f, 1.0f};    // Pink

        float playerSize = 40.0f;
        if (m_MouseLeft) playerSize = 50.0f;  // Grow when clicking

        Gfx::Renderer::DrawQuad({
            .Position = {m_PlayerPos.x, m_PlayerPos.y, 0.1f},
            .Size = {playerSize, playerSize},
            .Color = playerColor
        });

        // Draw mouse cursor indicator (in render target space)
        // This won't match exactly since mouse is in window coords, but shows the concept
        if (m_MouseRight)
        {
            Gfx::Renderer::DrawCircle({
                .Position = {m_PlayerPos.x, m_PlayerPos.y, 0.2f},
                .Radius = {30, 30},
                .Color = {1.0f, 1.0f, 0.0f, 0.5f},
                .Thickness = 0.1f
            });
        }

        // Draw direction indicator based on arrow keys
        glm::vec2 indicatorDir{0.0f};
        if (Input::IsKeyPressed(Input::KeyCode::Up)) indicatorDir.y -= 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::Down)) indicatorDir.y += 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::Left)) indicatorDir.x -= 1.0f;
        if (Input::IsKeyPressed(Input::KeyCode::Right)) indicatorDir.x += 1.0f;

        if (glm::length(indicatorDir) > 0.0f)
        {
            indicatorDir = glm::normalize(indicatorDir) * 40.0f;
            Gfx::Renderer::DrawLine({
                .Start = {m_PlayerPos.x, m_PlayerPos.y, 0.3f},
                .End = {m_PlayerPos.x + indicatorDir.x, m_PlayerPos.y + indicatorDir.y, 0.3f},
                .Color = {1.0f, 1.0f, 1.0f, 0.8f},
                .Thickness = 3.0f
            });
        }

        Gfx::Renderer::End();

        // UI
        RenderUI();
    }

    void RenderUI()
    {
        UI::BeginWindow("Input Demo");
        UI::Text("Input System Demo");
        UI::Separator();

        UI::Text("Keyboard:");
        UI::Text("  WASD/Arrows - Move square");
        UI::Text("  Shift - Orange color");
        UI::Text("  Ctrl - Green color");
        UI::Text("  Alt - Pink color");
        UI::Separator();

        UI::Text("Mouse:");
        UI::Text("  Left click - Enlarge square");
        UI::Text("  Right click - Show ring");
        UI::Separator();

        UI::Text("State:");
        UI::TextFmt("  Position: ({:.0f}, {:.0f})", m_PlayerPos.x, m_PlayerPos.y);
        UI::TextFmt("  Mouse: ({:.0f}, {:.0f})", m_MousePos.x, m_MousePos.y);
        UI::Separator();

        UI::Text("Modifiers:");
        UI::TextFmt("  Shift: {}", m_ShiftHeld ? "HELD" : "-");
        UI::TextFmt("  Ctrl: {}", m_CtrlHeld ? "HELD" : "-");
        UI::TextFmt("  Alt: {}", m_AltHeld ? "HELD" : "-");
        UI::Separator();

        UI::Text("Mouse Buttons:");
        UI::TextFmt("  Left: {}", m_MouseLeft ? "PRESSED" : "-");
        UI::TextFmt("  Right: {}", m_MouseRight ? "PRESSED" : "-");
        UI::TextFmt("  Middle: {}", m_MouseMiddle ? "PRESSED" : "-");
        UI::Separator();

        const auto& stats = Gfx::Renderer::GetStats();
        UI::TextFmt("Draw Calls: {}", stats.DrawCalls);
        Gfx::Renderer::ResetStats();

        UI::EndWindow();

        // Viewport
        UI::BeginWindow("Viewport");
        auto tex = Gfx::Renderer::GetTexture();
        if (tex)
        {
            UI::Image(tex->GetTexture(), UI::GetContentSize());
        }
        UI::EndWindow();
    }

private:
    glm::vec2 m_PlayerPos{256.0f, 256.0f};
    glm::vec2 m_MousePos{0.0f, 0.0f};

    bool m_ShiftHeld = false;
    bool m_CtrlHeld = false;
    bool m_AltHeld = false;

    bool m_MouseLeft = false;
    bool m_MouseRight = false;
    bool m_MouseMiddle = false;
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "application/01-input";
    auto* app = new Application(specs);
    app->PushLayer<InputLayer>();
    return app;
}
