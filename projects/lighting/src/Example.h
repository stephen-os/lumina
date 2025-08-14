#include <vector>
#include <iostream>
#include "imgui.h"
#include "Lumina/Core/Layer.h"
#include "Lumina/Utils/Timer.h"
#include "Lumina/Utils/UUID.h"

#include "Lumina/Lumina.h"

namespace Lumina
{

    enum class LightFalloffType : int {
        Linear = 0,
        Quadratic = 1,
        Custom = 2
    };

    struct LightData 
    {
        bool enabled = true;
        glm::vec3 position = { 0.0f, 0.0f, 0.2f };
        glm::vec3 color = { 1.0f, 1.0f, 1.0f };
        float intensity = 1.0f;
        float radius = 5.0f;
        LightBlendMode blendMode = LightBlendMode::Additive;
        LightFalloffType falloffType = LightFalloffType::Quadratic;
        std::string name = "Light";
    };

    class Example : public Layer
    {
    public:
        virtual void OnAttach() override
        {
            m_UniqueID = UUID::Generate();

            // Initialize camera
            m_Camera = CreateRef<OrthographicCamera>(-10.0f, 10.0f, -10.0f, 10.0f);
            m_Camera->SetPosition({ m_CameraPosition.x, m_CameraPosition.y, 1.0f });
            m_Camera->LookAt({ 0.0f, 0.0f, 0.0f });
            m_Camera->SetZoom(m_CameraZoom);

            // Initialize quad settings
            m_QuadPosition = { 0.0f, 0.0f, 0.0f };
            m_QuadSize = { 3.0f, 3.0f };
            m_QuadColor = { 1.0f, 1.0f, 1.0f, 1.0f };

            // Initialize lighting settings
            m_LightingEnabled = true;

            // Initialize ambient light
            m_AmbientColor = { 0.1f, 0.1f, 0.2f };
            m_AmbientIntensity = 0.3f;

            // Add some default lights
            AddDefaultLights();
        }

        virtual void OnDetach() override
        {
        }

        virtual void OnUpdate(float timestep) override
        {
            Renderer2D::ResetStats();

            // Update FPS
            float elapsedTime = m_FrameTimer.Elapsed();
            m_FPS = 1.0f / elapsedTime;
            m_FrameTimer.Reset();

            // Update camera if needed
            m_Camera->SetPosition({ m_CameraPosition.x, m_CameraPosition.y, 0.0f });
            m_Camera->SetZoom(m_CameraZoom);
        }

        virtual void OnUIRender() override
        {
            // Render scene
            Renderer2D::Begin(*m_Camera);

            // Enable/disable lighting
            Renderer2D::UseLighting(m_LightingEnabled);

            // Set ambient lighting
            Renderer2D::SetAmbientLightColor(m_AmbientColor);
            Renderer2D::SetAmbientLightIntensity(m_AmbientIntensity);

            // Draw all enabled lights
            if (m_LightingEnabled) {
                for (const auto& light : m_Lights) {
                    if (light.enabled) {
                        Renderer2D::SetPointLightPosition(light.position);
                        Renderer2D::SetPointLightColor({ light.color.x, light.color.y, light.color.z, 1.0f });
                        Renderer2D::SetPointLightIntensity(light.intensity);
                        Renderer2D::SetPointLightRadius(light.radius);
                        Renderer2D::SetPointLightBlendMode(light.blendMode);
                        Renderer2D::DrawPointLight();
                    }
                }
            }

            // Calculate grid positioning
            float gridWidth = (m_GridWidth - 1) * m_QuadSpacing;
            float gridHeight = (m_GridHeight - 1) * m_QuadSpacing;
            glm::vec3 gridOffset = { -gridWidth * 0.5f, -gridHeight * 0.5f, 0.0f };

            // Set consistent quad settings
            Renderer2D::SetQuadSize(m_QuadSize);
            Renderer2D::SetQuadTintColor(m_QuadColor);

            // Draw grid of quads
            for (int x = 0; x < m_GridWidth; x++) {
                for (int y = 0; y < m_GridHeight; y++) {
                    glm::vec3 quadPos = {
                        x * m_QuadSpacing + gridOffset.x,
                        y * m_QuadSpacing + gridOffset.y,
                        0.0f
                    };

                    Renderer2D::SetQuadPosition(quadPos);
                    Renderer2D::DrawQuad();
                }
            }

            Renderer2D::End();

            // UI Rendering
            RenderUI();
        }

    private:
        void AddDefaultLights()
        {
            // Red additive light
            LightData redLight;
            redLight.name = "Red Light";
            redLight.position = { 3.0f, 3.0f, 0.2f };
            redLight.color = { 1.0f, 0.0f, 0.0f };
            redLight.intensity = 2.0f;
            redLight.radius = 8.0f;
            redLight.blendMode = LightBlendMode::Additive;
            redLight.falloffType = LightFalloffType::Quadratic;
            m_Lights.push_back(redLight);

            // Blue shadow light
            LightData blueLight;
            blueLight.name = "Blue Shadow";
            blueLight.position = { -3.0f, -3.0f, 0.2f };
            blueLight.color = { 0.0f, 0.0f, 1.0f };
            blueLight.intensity = 1.5f;
            blueLight.radius = 6.0f;
            blueLight.blendMode = LightBlendMode::Multiply;
            blueLight.falloffType = LightFalloffType::Linear;
            m_Lights.push_back(blueLight);
        }

        void RenderUI()
        {
            // Viewport 
            ImGui::Begin("Viewport");
            ImVec2 size = ImGui::GetContentRegionAvail();
            m_Camera->SetSize(size.x, size.y);
            Renderer2D::SetResolution(size.x, size.y);
            ImGui::Image((void*)(intptr_t)Renderer2D::GetImage(), size);
            ImGui::End();

            // Performance
            ImGui::Begin("Performance");
            ImGui::Text("FPS: %.1f", m_FPS);
            auto stats = Renderer2D::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quad Count: %d", stats.QuadCount);
            ImGui::Text("Point Lights: %d", stats.PointLightCount);
            ImGui::Text("Lighting Enabled: %s", stats.LightingUsed ? "Yes" : "No");
            ImGui::End();

            // Camera Controls
            ImGui::Begin("Camera");
            ImGui::SliderFloat2("Position", &m_CameraPosition.x, -20.0f, 20.0f);
            ImGui::SliderFloat("Zoom", &m_CameraZoom, 1.0f, 200.0f);
            if (ImGui::Button("Reset Camera")) {
                m_CameraPosition = { 0.0f, 0.0f };
                m_CameraZoom = 100.0f;
            }
            ImGui::End();

            // Grid Controls
            ImGui::Begin("Grid Settings");
            ImGui::SliderInt("Grid Width", &m_GridWidth, 1, 20);
            ImGui::SliderInt("Grid Height", &m_GridHeight, 1, 20);
            ImGui::SliderFloat("Quad Spacing", &m_QuadSpacing, 1.0f, 10.0f);
            ImGui::SliderFloat2("Quad Size", &m_QuadSize.x, 0.1f, 5.0f);
            ImGui::ColorEdit4("Quad Color", &m_QuadColor.x);
            if (ImGui::Button("Reset Grid")) {
                m_GridWidth = 5;
                m_GridHeight = 5;
                m_QuadSpacing = 5.0f;
                m_QuadSize = { 3.0f, 3.0f };
                m_QuadColor = { 1.0f, 1.0f, 1.0f, 1.0f };
            }
            ImGui::End();

            // Global Lighting Controls
            ImGui::Begin("Global Lighting");
            ImGui::Checkbox("Enable Lighting", &m_LightingEnabled);

            ImGui::Separator();
            ImGui::Text("Ambient Light");
            ImGui::ColorEdit3("Ambient Color", &m_AmbientColor.x);
            ImGui::SliderFloat("Ambient Intensity", &m_AmbientIntensity, 0.0f, 2.0f);

            ImGui::Separator();
            ImGui::Text("Light Management");
            if (ImGui::Button("Add Light")) {
                AddNewLight();
            }
            ImGui::SameLine();
            if (ImGui::Button("Clear All Lights")) {
                m_Lights.clear();
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset to Defaults")) {
                m_Lights.clear();
                AddDefaultLights();
            }

            ImGui::Text("Total Lights: %d / 32", (int)m_Lights.size());
            ImGui::End();

            // Individual Light Controls
            ImGui::Begin("Light Controls");
            for (size_t i = 0; i < m_Lights.size(); i++) {
                auto& light = m_Lights[i];

                ImGui::PushID((int)i);

                // Light header with delete button
                ImGui::Text("%s #%d", light.name.c_str(), (int)i);
                ImGui::SameLine();
                if (ImGui::SmallButton("Delete")) {
                    m_Lights.erase(m_Lights.begin() + i);
                    ImGui::PopID();
                    break; // Exit loop since we modified the vector
                }

                // Enable/disable checkbox
                ImGui::Checkbox("Enabled", &light.enabled);

                if (light.enabled) {
                    // Position
                    ImGui::SliderFloat3("Position", &light.position.x, -15.0f, 15.0f);

                    // Color and intensity
                    ImGui::ColorEdit3("Color", &light.color.x);
                    ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 10.0f);
                    ImGui::SliderFloat("Radius", &light.radius, 0.1f, 20.0f);

                    // Blend mode
                    const char* blendModes[] = { "Additive", "Multiply" };
                    int currentBlend = (int)light.blendMode;
                    if (ImGui::Combo("Blend Mode", &currentBlend, blendModes, 2)) {
                        light.blendMode = (LightBlendMode)currentBlend;
                    }

                    // Falloff type
                    const char* falloffTypes[] = { "Linear", "Quadratic", "Custom" };
                    int currentFalloff = (int)light.falloffType;
                    if (ImGui::Combo("Falloff Type", &currentFalloff, falloffTypes, 3)) {
                        light.falloffType = (LightFalloffType)currentFalloff;
                    }

                    // Quick presets
                    if (ImGui::Button("Red Light")) {
                        light.color = { 1.0f, 0.0f, 0.0f };
                        light.blendMode = LightBlendMode::Additive;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Blue Shadow")) {
                        light.color = { 0.0f, 0.0f, 1.0f };
                        light.blendMode = LightBlendMode::Multiply;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("White Light")) {
                        light.color = { 1.0f, 1.0f, 1.0f };
                        light.blendMode = LightBlendMode::Additive;
                    }
                }

                ImGui::Separator();
                ImGui::PopID();
            }
            ImGui::End();

            // Debug Info
            ImGui::Begin("Debug Info");
            ImGui::Text("Unique ID: %llu", m_UniqueID);
            ImGui::Text("Camera Position: (%.2f, %.2f)", m_CameraPosition.x, m_CameraPosition.y);
            ImGui::Text("Total Quads: %d", m_GridWidth * m_GridHeight);
            ImGui::Text("Active Lights: %d", GetActiveLightCount());
            ImGui::Text("Lighting System: %s", m_LightingEnabled ? "Enabled" : "Disabled");
            ImGui::End();
        }

        void AddNewLight()
        {
            if (m_Lights.size() >= 32) {
                // Could show a popup or just ignore
                return;
            }

            LightData newLight;
            newLight.name = "Light";
            newLight.position = { 0.0f, 0.0f, 0.0f };
            newLight.color = { 1.0f, 1.0f, 1.0f };
            newLight.intensity = 1.0f;
            newLight.radius = 5.0f;
            newLight.blendMode = LightBlendMode::Additive;
            newLight.falloffType = LightFalloffType::Quadratic;
            m_Lights.push_back(newLight);
        }

        int GetActiveLightCount() const
        {
            int count = 0;
            for (const auto& light : m_Lights) {
                if (light.enabled) count++;
            }
            return count;
        }

    private:
        Timer m_FrameTimer;
        float m_FPS = 0.0f;
        uint64_t m_UniqueID = 0;

        Ref<OrthographicCamera> m_Camera;
        glm::vec2 m_CameraPosition = { 0.0f, 0.0f };
        float m_CameraZoom = 100.0f;

        // Grid settings
        int m_GridWidth = 5;
        int m_GridHeight = 5;
        float m_QuadSpacing = 5.0f;

        // Quad settings
        glm::vec3 m_QuadPosition;
        glm::vec2 m_QuadSize;
        glm::vec4 m_QuadColor;

        // Lighting settings
        bool m_LightingEnabled;
        glm::vec3 m_AmbientColor;
        float m_AmbientIntensity;

        // Light management
        std::vector<LightData> m_Lights;
    };
}