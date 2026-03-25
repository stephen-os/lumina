// 06-lighting: 2D Lighting Demo
// Demonstrates: Point lights with various attenuation models, ambient light

#include <Lumina/Core/Core.h>
#include <Lumina/Core/EntryPoint.h>
#include <Lumina/Core/Input.h>
#include <Lumina/Graphics/Graphics.h>
#include <Lumina/UI/UI.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace UI = Lumina::UI;
namespace Input = Lumina::Input;
namespace Gfx = Lumina::Graphics;

class LightingLayer : public Lumina::Layer
{
public:
    LightingLayer() : Layer("Lighting") {}

    void OnAttach() override
    {
        auto& device = Lumina::Application::Get().GetDevice();
        m_Renderer = std::make_unique<Gfx::Renderer2D>(device);
        m_Renderer->Init();

        // Create initial render target (will be resized to match viewport)
        m_RenderTarget = Gfx::RenderTarget::Create(
            device, 800, 600, Gfx::Format::RGBA8_UNORM
        );

        LUMINA_LOG_INFO("Lighting demo attached");
    }

    void OnDetach() override
    {
        m_RenderTarget.reset();
        m_Renderer.reset();
    }

    void OnUpdate(float dt) override
    {
        m_Time += dt;

        // Camera controls
        float moveSpeed = 200.0f * dt;
        if (Input::IsKeyPressed(Input::KeyCode::W))
            m_CameraPos.y -= moveSpeed / m_Zoom;
        if (Input::IsKeyPressed(Input::KeyCode::S))
            m_CameraPos.y += moveSpeed / m_Zoom;
        if (Input::IsKeyPressed(Input::KeyCode::A))
            m_CameraPos.x -= moveSpeed / m_Zoom;
        if (Input::IsKeyPressed(Input::KeyCode::D))
            m_CameraPos.x += moveSpeed / m_Zoom;

        // Animate mouse light position towards cursor (when in viewport)
        if (m_FollowMouse)
        {
            m_MouseLightPos = m_MouseWorldPos;
        }
    }

    void OnRender() override
    {
        if (!m_Renderer) return;

        // UI Controls window
        RenderControlsUI();

        // Viewport window - handles render target sizing, rendering, and display
        RenderViewport();
    }

    void RenderScene(float width, float height)
    {
        // Create view matrix from camera position and zoom
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(width / 2.0f, height / 2.0f, 0.0f));
        view = glm::scale(view, glm::vec3(m_Zoom, m_Zoom, 1.0f));
        view = glm::translate(view, glm::vec3(-m_CameraPos.x, -m_CameraPos.y, 0.0f));

        glm::mat4 projection = glm::ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

        // Enable lighting
        m_Renderer->SetLightingEnabled(m_LightingEnabled);
        m_Renderer->SetAmbientLight(m_AmbientColor, m_AmbientIntensity);

        m_Renderer->Begin(view, projection);
        m_Renderer->SetRenderTarget(m_RenderTarget);

        // Draw background - extends beyond viewport for camera panning
        m_Renderer->DrawQuad({
            .Position = {m_CameraPos.x - width * 2, m_CameraPos.y - height * 2, -0.1f},
            .Size = {width * 5, height * 5},
            .Color = {0.15f, 0.15f, 0.2f, 1.0f}
        });

        // Draw a grid pattern to show lighting better
        for (int y = 0; y < 12; y++)
        {
            for (int x = 0; x < 20; x++)
            {
                float px = 40.0f + x * 80.0f;
                float py = 40.0f + y * 80.0f;

                glm::vec4 color = ((x + y) % 2 == 0)
                    ? glm::vec4(0.4f, 0.4f, 0.5f, 1.0f)
                    : glm::vec4(0.3f, 0.3f, 0.4f, 1.0f);

                m_Renderer->DrawQuad({
                    .Position = {px, py, 0},
                    .Size = {70, 70},
                    .Color = color
                });
            }
        }

        // Draw some colored objects
        m_Renderer->DrawCircle({
            .Position = {300, 200, 0},
            .Radius = {50, 50},
            .Color = {0.8f, 0.2f, 0.2f, 1.0f}
        });

        m_Renderer->DrawCircle({
            .Position = {width / 2, height / 2, 0},
            .Radius = {70, 70},
            .Color = {0.2f, 0.8f, 0.2f, 1.0f}
        });

        m_Renderer->DrawQuad({
            .Position = {width - 330, height - 200, 0},
            .Size = {120, 100},
            .Color = {0.2f, 0.2f, 0.8f, 1.0f},
            .Rotation = m_Time * 0.5f
        });

        m_Renderer->DrawTriangle({
            .P0 = {450, 480, 0},
            .P1 = {380, 600, 0},
            .P2 = {520, 600, 0},
            .Color = {0.8f, 0.8f, 0.2f, 1.0f}
        });

        // Add point lights
        if (m_LightingEnabled)
        {
            float centerX = width / 2;
            float centerY = height / 2;

            // Orbiting red light
            float orbitX = centerX + std::cos(m_Time) * 250;
            float orbitY = centerY + std::sin(m_Time) * 180;
            m_Renderer->DrawPointLight({
                .Position = {orbitX, orbitY, 0},
                .Color = {1.0f, 0.3f, 0.3f},
                .Intensity = m_LightIntensity,
                .Radius = m_LightRadius,
                .Attenuation = m_AttenuationModel
            });

            // Orbiting blue light (opposite phase)
            float orbitX2 = centerX + std::cos(m_Time + 3.14159f) * 250;
            float orbitY2 = centerY + std::sin(m_Time + 3.14159f) * 180;
            m_Renderer->DrawPointLight({
                .Position = {orbitX2, orbitY2, 0},
                .Color = {0.3f, 0.3f, 1.0f},
                .Intensity = m_LightIntensity,
                .Radius = m_LightRadius,
                .Attenuation = m_AttenuationModel
            });

            // Static green light at center
            m_Renderer->DrawPointLight({
                .Position = {centerX, centerY, 0},
                .Color = {0.3f, 1.0f, 0.3f},
                .Intensity = m_LightIntensity * 0.5f,
                .Radius = m_LightRadius * 1.5f,
                .Attenuation = m_AttenuationModel
            });

            // Mouse-following light
            if (m_ShowMouseLight)
            {
                m_Renderer->DrawPointLight({
                    .Position = {m_MouseLightPos.x, m_MouseLightPos.y, 0},
                    .Color = m_MouseLightColor,
                    .Intensity = m_LightIntensity,
                    .Radius = m_LightRadius,
                    .Attenuation = m_AttenuationModel
                });
            }
        }

        m_Renderer->End();
    }

    void RenderControlsUI()
    {
        UI::BeginWindow("Lighting Controls");
        UI::Text("2D Lighting Demo");
        UI::Separator();

        UI::Text("Controls: WASD = Pan");
        UI::TextFmt("Camera: ({:.0f}, {:.0f})", m_CameraPos.x, m_CameraPos.y);
        UI::TextFmt("Viewport: {}x{}", m_ViewportWidth, m_ViewportHeight);
        UI::Separator();

        UI::Checkbox("Enable Lighting", m_LightingEnabled);
        UI::Separator();

        if (m_LightingEnabled)
        {
            UI::Text("Ambient Light");
            UI::PropertyColor3("Ambient Color", glm::value_ptr(m_AmbientColor));
            UI::PropertySlider("Ambient Intensity", m_AmbientIntensity, 0.0f, 2.0f);
            UI::Separator();

            UI::Text("Point Lights");
            UI::PropertySlider("Radius", m_LightRadius, 50.0f, 500.0f);
            UI::PropertySlider("Light Intensity", m_LightIntensity, 0.1f, 5.0f);

            // Attenuation model dropdown
            static const char* attenuationNames[] = {
                "None", "Linear", "Quadratic", "Inverse Square",
                "Exponential", "Smoothstep", "Realistic"
            };
            int current = static_cast<int>(m_AttenuationModel);
            if (UI::PropertyDropdown("Attenuation", current, attenuationNames, 7))
            {
                m_AttenuationModel = static_cast<Gfx::AttenuationModel>(current);
            }
            UI::Separator();

            UI::Text("Mouse Light");
            UI::Checkbox("Show Mouse Light", m_ShowMouseLight);
            UI::Checkbox("Follow Mouse", m_FollowMouse);
            if (m_ShowMouseLight)
            {
                UI::PropertyColor3("Mouse Color", glm::value_ptr(m_MouseLightColor));
            }
        }
        UI::Separator();

        const auto& stats = m_Renderer->GetStats();
        UI::TextFmt("Draw Calls: {}", stats.DrawCalls);
        UI::TextFmt("Point Lights: {}", stats.PointLightCount);
        m_Renderer->ResetStats();

        UI::EndWindow();
    }

    void RenderViewport()
    {
        // Viewport window - remove padding for clean image display
        UI::PushStyleVar(ImGuiStyleVar_WindowPadding, glm::vec2(0, 0));
        UI::BeginWindow("Viewport");

        // Get available content size
        auto contentSize = UI::GetContentSize();
        uint32_t newWidth = static_cast<uint32_t>(std::max(1.0f, contentSize.x));
        uint32_t newHeight = static_cast<uint32_t>(std::max(1.0f, contentSize.y));

        // Resize render target if viewport size changed
        if (newWidth != m_ViewportWidth || newHeight != m_ViewportHeight)
        {
            m_ViewportWidth = newWidth;
            m_ViewportHeight = newHeight;

            if (m_RenderTarget)
            {
                m_RenderTarget->Resize(m_ViewportWidth, m_ViewportHeight);
            }
        }

        // Render the scene at viewport size
        if (m_RenderTarget && m_ViewportWidth > 0 && m_ViewportHeight > 0)
        {
            RenderScene(static_cast<float>(m_ViewportWidth), static_cast<float>(m_ViewportHeight));
        }

        // Display at 1:1 scale
        auto tex = m_RenderTarget ? m_RenderTarget->GetColorTexture() : nullptr;
        UI::ImageRect imgRect = {};
        if (tex)
        {
            imgRect = UI::ImageWithRect(tex->GetTexture(), contentSize);
        }

        // Calculate mouse position in world coordinates
        auto mousePos = UI::GetMousePos();
        if (UI::IsItemHovered())
        {
            // At 1:1 scale, local position IS render target position
            float rtX = mousePos.x - imgRect.Pos.x;
            float rtY = mousePos.y - imgRect.Pos.y;

            // Convert to world coordinates considering camera
            float halfWidth = static_cast<float>(m_ViewportWidth) / 2.0f;
            float halfHeight = static_cast<float>(m_ViewportHeight) / 2.0f;
            m_MouseWorldPos.x = m_CameraPos.x + (rtX - halfWidth) / m_Zoom;
            m_MouseWorldPos.y = m_CameraPos.y + (rtY - halfHeight) / m_Zoom;
        }

        UI::EndWindow();
        UI::PopStyleVar();
    }

private:
    std::unique_ptr<Gfx::Renderer2D> m_Renderer;
    Lumina::Ref<Gfx::RenderTarget> m_RenderTarget;
    float m_Time = 0.0f;

    // Viewport state
    uint32_t m_ViewportWidth = 0;
    uint32_t m_ViewportHeight = 0;

    // Camera state
    glm::vec2 m_CameraPos = {400.0f, 300.0f};
    float m_Zoom = 1.0f;

    // Lighting settings
    bool m_LightingEnabled = true;
    glm::vec3 m_AmbientColor = {0.1f, 0.1f, 0.15f};
    float m_AmbientIntensity = 1.0f;
    float m_LightRadius = 200.0f;
    float m_LightIntensity = 1.5f;
    Gfx::AttenuationModel m_AttenuationModel = Gfx::AttenuationModel::Quadratic;

    // Mouse light
    bool m_ShowMouseLight = true;
    bool m_FollowMouse = true;
    glm::vec3 m_MouseLightColor = {1.0f, 0.9f, 0.7f};
    glm::vec2 m_MouseLightPos = {400, 300};
    glm::vec2 m_MouseWorldPos = {400, 300};
};

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    ApplicationSpecifications specs;
    specs.Title = "graphics/06-lighting";
    auto* app = new Application(specs);
    app->PushLayer<LightingLayer>();
    return app;
}
