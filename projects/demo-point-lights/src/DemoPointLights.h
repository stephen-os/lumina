#include <vector>
#include <iostream>
#include "imgui.h"
#include "Lumina/Core/Layer.h"
#include "Lumina/Utils/Timer.h"
#include "Lumina/Utils/UUID.h"

#include "Lumina/Lumina.h"

namespace Lumina
{
    struct LightData
    {
        bool enabled = true;
        glm::vec3 position = { 0.0f, 0.0f, 0.2f };
        glm::vec3 color = { 1.0f, 1.0f, 1.0f };
        float intensity = 1.0f;
        float radius = 5.0f;
        BlendMode blendMode = BlendMode::Additive;
        float blendAlpha = 1.0f;
        AttenuationModel falloffType = AttenuationModel::Quadratic;
        float falloffParam = 1.0f;
        std::string name = "Light";

        // Animation settings
        bool animatePosition = false;
        bool animateIntensity = false;
        bool animateColor = false;
        float animationSpeed = 1.0f;
        float animationTime = 0.0f;
    };

    enum class SceneType
    {
        QuadGrid = 0,
        Circles = 1,
        Lines = 2,
        Triangles = 3,
        Text = 4,
        Pixels = 5,
        Mixed = 6
    };

    class DemoPointLights : public Layer
    {
    public:
        virtual void OnAttach() override
        {
            m_Camera = CreateRef<OrthographicCamera>(-10.0f, 10.0f, -10.0f, 10.0f);
            m_Camera->SetPosition({ m_CameraPosition.x, m_CameraPosition.y, 1.0f });
            m_Camera->LookAt({ 0.0f, 0.0f, 0.0f });
            m_Camera->SetZoom(m_CameraZoom);

            AddTestLights();
        }

        virtual void OnUpdate(float timestep) override
        {
            Renderer2D::ResetStats();

            float elapsedTime = m_FrameTimer.Elapsed();
            m_FPS = 1.0f / elapsedTime;
            m_FrameTimer.Reset();

            m_Camera->SetPosition({ m_CameraPosition.x, m_CameraPosition.y, 0.0f });
            m_Camera->SetZoom(m_CameraZoom);

            for (auto& light : m_Lights) 
            {
                if (light.enabled) 
                {
                    light.animationTime += timestep * light.animationSpeed;

                    if (light.animatePosition) 
                    {
                        float radius = 8.0f;
                        light.position.x = cos(light.animationTime) * radius;
                        light.position.y = sin(light.animationTime) * radius;
                    }

                    if (light.animateIntensity) 
                    {
                        light.intensity = 1.0f + sin(light.animationTime * 2.0f) * 0.8f;
                    }

                    if (light.animateColor) 
                    {
                        light.color.r = 0.5f + 0.5f * sin(light.animationTime);
                        light.color.g = 0.5f + 0.5f * sin(light.animationTime + 2.0f);
                        light.color.b = 0.5f + 0.5f * sin(light.animationTime + 4.0f);
                    }
                }
            }
        }

        virtual void OnUIRender() override
        {
            Renderer2D::Begin(m_Camera);

            Renderer2D::UseLighting(m_LightingEnabled);

            Renderer2D::SetAmbientLightColor(m_AmbientColor);
            Renderer2D::SetAmbientLightIntensity(m_AmbientIntensity);

            if (m_LightingEnabled) 
            {
                for (const auto& light : m_Lights) 
                {
                    if (light.enabled) 
                    {
                        Renderer2D::SetPointLightPosition(light.position);
                        Renderer2D::SetPointLightColor({ light.color.x, light.color.y, light.color.z, 1.0f });
                        Renderer2D::SetPointLightIntensity(light.intensity);
                        Renderer2D::SetPointLightRadius(light.radius);
                        Renderer2D::SetPointLightBlendMode(light.blendMode);
                        Renderer2D::SetPointLightBlendAlpha(light.blendAlpha);
                        Renderer2D::SetPointLightFalloffType(light.falloffType);
                        Renderer2D::SetPointLightFalloff(light.falloffParam);
                        Renderer2D::DrawPointLight();
                    }
                }
            }

            DrawCurrentScene();

            Renderer2D::End();

            RenderUI();
        }

    private:
        void DrawCurrentScene()
        {
            switch (m_CurrentScene)
            {
            case SceneType::QuadGrid:
                DrawQuadGridScene();
                break;
            case SceneType::Circles:
                DrawCircleScene();
                break;
            case SceneType::Lines:
                DrawLineScene();
                break;
            case SceneType::Triangles:
                DrawTriangleScene();
                break;
            case SceneType::Text:
                DrawTextScene();
                break;
            case SceneType::Pixels:
                DrawPixelScene();
                break;
            case SceneType::Mixed:
                DrawMixedScene();
                break;
            }
        }

        void DrawQuadGridScene()
        {
            float gridWidth = (m_GridWidth - 1) * m_QuadSpacing;
            float gridHeight = (m_GridHeight - 1) * m_QuadSpacing;
            glm::vec3 gridOffset = { -gridWidth * 0.5f, -gridHeight * 0.5f, 0.0f };

            Renderer2D::SetQuadSize(m_QuadSize);
            Renderer2D::SetQuadTintColor(m_QuadColor);

            for (int x = 0; x < m_GridWidth; x++) 
            {
                for (int y = 0; y < m_GridHeight; y++) 
                {
                    glm::vec3 quadPos = {
                        x * m_QuadSpacing + gridOffset.x + m_QuadPositionOffset.x,
                        y * m_QuadSpacing + gridOffset.y + m_QuadPositionOffset.y,
                        m_QuadPositionOffset.z
                    };

                    Renderer2D::SetQuadPosition(quadPos);
                    Renderer2D::DrawQuad();
                }
            }
        }

        void DrawCircleScene()
        {
            float gridWidth = (m_GridWidth - 1) * m_QuadSpacing;
            float gridHeight = (m_GridHeight - 1) * m_QuadSpacing;
            glm::vec3 gridOffset = { -gridWidth * 0.5f, -gridHeight * 0.5f, 0.0f };

            Renderer2D::SetCircleRadius(m_CircleRadius);
            Renderer2D::SetCircleColor(m_CircleColor);
            Renderer2D::SetCircleThickness(m_CircleThickness);
            Renderer2D::SetCircleFade(m_CircleFade);

            for (int x = 0; x < m_GridWidth; x++) 
            {
                for (int y = 0; y < m_GridHeight; y++) 
                {
                    glm::vec3 circlePos = {
                        x * m_QuadSpacing + gridOffset.x,
                        y * m_QuadSpacing + gridOffset.y,
                        0.0f
                    };

                    Renderer2D::SetCirclePosition(circlePos);
                    Renderer2D::DrawCircle();
                }
            }
        }

        void DrawLineScene()
        {
            Renderer2D::SetLineColor(m_LineColor);
            Renderer2D::SetLineThickness(m_LineThickness);

            for (int i = 0; i < m_LineCount; i++) 
            {
                float angle = (i / (float)m_LineCount) * 2.0f * 3.14159f;
                glm::vec3 start = { 0.0f, 0.0f, 0.0f };
                glm::vec3 end = { cos(angle) * m_LineLength, sin(angle) * m_LineLength, 0.0f };

                Renderer2D::SetLineStart(start);
                Renderer2D::SetLineEnd(end);
                Renderer2D::DrawLine();
            }

            for (int ring = 1; ring <= 3; ring++) 
            {
                float radius = ring * 3.0f;
                int segments = 32;
                for (int i = 0; i < segments; i++) 
                {
                    float angle1 = (i / (float)segments) * 2.0f * 3.14159f;
                    float angle2 = ((i + 1) / (float)segments) * 2.0f * 3.14159f;

                    glm::vec3 start = { cos(angle1) * radius, sin(angle1) * radius, 0.0f };
                    glm::vec3 end = { cos(angle2) * radius, sin(angle2) * radius, 0.0f };

                    Renderer2D::SetLineStart(start);
                    Renderer2D::SetLineEnd(end);
                    Renderer2D::DrawLine();
                }
            }
        }

        void DrawTriangleScene()
        {
            Renderer2D::SetTriangleColor(m_TriangleColor);

            for (int x = -3; x <= 3; x++) 
            {
                for (int y = -3; y <= 3; y++) 
                {
                    glm::vec3 center = { x * 4.0f, y * 4.0f, 0.0f };

                    // Upward triangle
                    Renderer2D::SetTrianglePoint1({ center.x, center.y + m_TriangleSize, 0.0f });
                    Renderer2D::SetTrianglePoint2({ center.x - m_TriangleSize, center.y - m_TriangleSize, 0.0f });
                    Renderer2D::SetTrianglePoint3({ center.x + m_TriangleSize, center.y - m_TriangleSize, 0.0f });
                    Renderer2D::DrawTriangle();

                    glm::vec3 offsetCenter = { center.x + 2.0f, center.y - 2.0f, 0.0f };
                    Renderer2D::SetTrianglePoint1({ offsetCenter.x, offsetCenter.y - m_TriangleSize, 0.0f });
                    Renderer2D::SetTrianglePoint2({ offsetCenter.x - m_TriangleSize, offsetCenter.y + m_TriangleSize, 0.0f });
                    Renderer2D::SetTrianglePoint3({ offsetCenter.x + m_TriangleSize, offsetCenter.y + m_TriangleSize, 0.0f });
                    Renderer2D::DrawTriangle();
                }
            }
        }

        void DrawTextScene()
        {
            Renderer2D::SetStringColor(m_TextColor);
            Renderer2D::SetStringSize(m_TextSize);

            std::vector<std::string> words = 
            { 
                "LUMINA", 
                "ENGINE", 
                "2D", 
                "RENDER" 
            };
            
            Renderer2D::SetStringAlignment(StringAlignment::Center);

            for (int i = 0; i < words.size(); i++) 
            {
                Renderer2D::SetStringContent(words[i]);
                Renderer2D::SetStringPosition({ 0.0f, i * -2.0f - 4.0f, 0.0f });
                Renderer2D::DrawString();
            }

            if (m_TextContent.size() > 0) 
            {
                Renderer2D::SetStringContent(std::string(m_TextContent));
                Renderer2D::SetStringPosition({ 0.0f, 0.0f, 0.0f });
                Renderer2D::SetStringSize(m_TextSize);
                Renderer2D::DrawString();
            }

			std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            Renderer2D::SetStringSize(1.0f);

            for (int row = 0; row < 4; row++) 
            {
                int startIdx = row * 8;
                int length = std::min(8, (int)alphabet.length() - startIdx);

                if (length > 0) 
                {
                    std::string rowText = alphabet.substr(startIdx, length);

                    Renderer2D::SetStringContent(rowText);
                    Renderer2D::SetStringPosition({ 0.0f, 10.0f + row * -2.0f, 0.0f });
                    Renderer2D::DrawString();
                }
            }
        }

        void DrawPixelScene()
        {
            Renderer2D::SetPixelColor(m_PixelColor);
            Renderer2D::SetPixelSize(m_PixelSize);

            for (int x = -m_PixelDensity; x <= m_PixelDensity; x++) 
            {
                for (int y = -m_PixelDensity; y <= m_PixelDensity; y++) 
                {
                    double distance = sqrt(x * x + y * y);

                    // Create various patterns
                    bool shouldDraw = false;

                    switch (m_PixelPattern) {
                    case 0: // Circle pattern
                        shouldDraw = (distance <= m_PixelDensity) &&
                            (fmod(distance, 3.0f) < 1.5f);
                        break;
                    case 1: // Checkerboard
                        shouldDraw = ((x + y) % 2) == 0;
                        break;
                    case 2: // Cross pattern
                        shouldDraw = (x == 0 || y == 0 || abs(x) == abs(y));
                        break;
                    case 3: // Random pattern
                        shouldDraw = ((x * 7 + y * 13) % 5) == 0;
                        break;
                    }

                    if (shouldDraw) 
                    {
                        Renderer2D::SetPixelPosition({ x * 0.5f, y * 0.5f, 0.0f });
                        Renderer2D::DrawPixel();
                    }
                }
            }
        }

        void DrawMixedScene()
        {
            // Central quad
            Renderer2D::SetQuadPosition({ 0.0f, 0.0f, 0.0f });
            Renderer2D::SetQuadSize({ 3.0f, 3.0f });
            Renderer2D::SetQuadTintColor({ 0.8f, 0.8f, 0.8f, 1.0f });
            Renderer2D::DrawQuad();

            // Surrounding circles
            for (int i = 0; i < 6; i++) 
            {
                float angle = (i / 6.0f) * 2.0f * 3.14159f;
                float radius = 6.0f;

                Renderer2D::SetCirclePosition({ cos(angle) * radius, sin(angle) * radius, 0.0f });
                Renderer2D::SetCircleRadius({ 1.0f, 1.0f });
                Renderer2D::SetCircleColor({ 0.5f + 0.5f * cos(angle),  0.5f + 0.5f * sin(angle), 0.8f, 1.0f });
                Renderer2D::SetCircleThickness(0.7f);
                Renderer2D::DrawCircle();
            }

            // Connecting lines
            Renderer2D::SetLineThickness(2.0f);
            Renderer2D::SetLineColor({ 0.3f, 0.7f, 1.0f, 0.8f });
            for (int i = 0; i < 6; i++) 
            {
                float angle = (i / 6.0f) * 2.0f * 3.14159f;
                float radius = 6.0f;

                Renderer2D::SetLineStart({ 0.0f, 0.0f, 0.0f });
                Renderer2D::SetLineEnd({ cos(angle) * radius, sin(angle) * radius, 0.0f });
                Renderer2D::DrawLine();
            }

            float corners[4][2] = { {-10, -10}, {10, -10}, {10, 10}, {-10, 10} };
            Renderer2D::SetTriangleColor({ 1.0f, 0.3f, 0.7f, 0.7f });

            for (int i = 0; i < 4; i++) 
            {
                glm::vec3 center = { corners[i][0], corners[i][1], 0.0f };
                Renderer2D::SetTrianglePoint1({ center.x, center.y + 2.0f, 0.0f });
                Renderer2D::SetTrianglePoint2({ center.x - 2.0f, center.y - 2.0f, 0.0f });
                Renderer2D::SetTrianglePoint3({ center.x + 2.0f, center.y - 2.0f, 0.0f });
                Renderer2D::DrawTriangle();
            }

			Renderer2D::SetStringAlignment(StringAlignment::Center);
            Renderer2D::SetStringContent("MIXED");
            Renderer2D::SetStringPosition({ 0.0f, -12.0f, 0.0f });
            Renderer2D::SetStringSize(1.5f);
            Renderer2D::SetStringColor({ 1.0f, 1.0f, 0.3f, 1.0f });
            Renderer2D::DrawString();

            Renderer2D::SetPixelSize(3.0f);
            Renderer2D::SetPixelColor({ 0.9f, 0.2f, 0.9f, 1.0f });
            for (int i = -15; i <= 15; i += 3) 
            {
                Renderer2D::SetPixelPosition({ i, 12.0f, 0.0f });
                Renderer2D::DrawPixel();
                Renderer2D::SetPixelPosition({ i, -15.0f, 0.0f });
                Renderer2D::DrawPixel();
                Renderer2D::SetPixelPosition({ 12.0f, i, 0.0f });
                Renderer2D::DrawPixel();
                Renderer2D::SetPixelPosition({ -15.0f, i, 0.0f });
                Renderer2D::DrawPixel();
            }
        }

        void AddTestLights()
        {
            LightData additive;
            additive.name = "Additive Red";
            additive.position = { 5.0f, 5.0f, 0.2f };
            additive.color = { 1.0f, 0.2f, 0.2f };
            additive.intensity = 2.0f;
            additive.radius = 8.0f;
            additive.blendMode = BlendMode::Additive;
            additive.blendAlpha = 1.0f;
            additive.falloffType = AttenuationModel::Quadratic;
            additive.falloffParam = 1.0f;
            m_Lights.push_back(additive);

            LightData multiply;
            multiply.name = "Multiply Blue";
            multiply.position = { -5.0f, -5.0f, 0.2f };
            multiply.color = { 0.3f, 0.3f, 1.0f };
            multiply.intensity = 1.5f;
            multiply.radius = 6.0f;
            multiply.blendMode = BlendMode::Multiply;
            multiply.blendAlpha = 0.8f;
            multiply.falloffType = AttenuationModel::Linear;
            multiply.falloffParam = 1.0f;
            m_Lights.push_back(multiply);

            LightData screen;
            screen.name = "Screen Yellow";
            screen.position = { 5.0f, -5.0f, 0.2f };
            screen.color = { 1.0f, 1.0f, 0.2f };
            screen.intensity = 1.8f;
            screen.radius = 7.0f;
            screen.blendMode = BlendMode::Screen;
            screen.blendAlpha = 0.9f;
            screen.falloffType = AttenuationModel::Smoothstep;
            screen.falloffParam = 1.0f;
            m_Lights.push_back(screen);

            LightData subtract;
            subtract.name = "Subtract Purple";
            subtract.position = { -5.0f, 5.0f, 0.2f };
            subtract.color = { 0.8f, 0.2f, 0.8f };
            subtract.intensity = 1.2f;
            subtract.radius = 5.0f;
            subtract.blendMode = BlendMode::Subtract;
            subtract.blendAlpha = 0.7f;
            subtract.falloffType = AttenuationModel::Exponential;
            subtract.falloffParam = 0.3f;
            m_Lights.push_back(subtract);

            LightData orbiting;
            orbiting.name = "Orbiting Green";
            orbiting.position = { 0.0f, 0.0f, 0.2f };
            orbiting.color = { 0.2f, 1.0f, 0.2f };
            orbiting.intensity = 1.5f;
            orbiting.radius = 6.0f;
            orbiting.blendMode = BlendMode::Overlay;
            orbiting.blendAlpha = 0.8f;
            orbiting.falloffType = AttenuationModel::InverseSquare;
            orbiting.falloffParam = 1.0f;
            orbiting.animatePosition = true;
            orbiting.animationSpeed = 1.5f;
            m_Lights.push_back(orbiting);

            LightData pulsing;
            pulsing.name = "Pulsing Orange";
            pulsing.position = { 0.0f, 8.0f, 0.2f };
            pulsing.color = { 1.0f, 0.6f, 0.1f };
            pulsing.intensity = 1.0f;
            pulsing.radius = 9.0f;
            pulsing.blendMode = BlendMode::SoftLight;
            pulsing.blendAlpha = 0.6f;
            pulsing.falloffType = AttenuationModel::Custom;
            pulsing.falloffParam = 2.0f;
            pulsing.animateIntensity = true;
            pulsing.animationSpeed = 2.0f;
            m_Lights.push_back(pulsing);
        }

        void RenderUI()
        {
            ImGui::Begin("Viewport");
            ImVec2 size = ImGui::GetContentRegionAvail();
            m_Camera->SetSize(size.x, size.y);
            Renderer2D::SetResolution((uint32_t)size.x, (uint32_t)size.y);
            ImGui::Image((void*)(intptr_t)Renderer2D::GetImage(), size);
            ImGui::End();

            ImGui::Begin("Performance");
            ImGui::Text("FPS: %.1f", m_FPS);
            auto stats = Renderer2D::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quads: %d | Circles: %d", stats.QuadCount, stats.CircleCount);
            ImGui::Text("Lines: %d | Triangles: %d", stats.LineCount, stats.TriangleCount);
            ImGui::Text("Text: %d | Pixels: %d", stats.TextCount, stats.PixelCount);
            ImGui::Text("Point Lights: %d", stats.PointLightCount);
            ImGui::Text("Lighting: %s", stats.LightingUsed ? "ON" : "OFF");
            ImGui::End();

            ImGui::Begin("Scene Selection");
            const char* sceneNames[] = 
            { 
                "Quad Grid", 
                "Circles", 
                "Lines", 
                "Triangles", 
                "Text", 
                "Pixels", 
                "Mixed" 
            };
            int currentScene = (int)m_CurrentScene;
            if (ImGui::Combo("Current Scene", &currentScene, sceneNames, 7)) 
            {
                m_CurrentScene = (SceneType)currentScene;
            }

            if (ImGui::Button("Quad Grid", ImVec2(-1, 0))) m_CurrentScene = SceneType::QuadGrid;
            if (ImGui::Button("Circles Grid", ImVec2(-1, 0))) m_CurrentScene = SceneType::Circles;
            if (ImGui::Button("Lines", ImVec2(-1, 0))) m_CurrentScene = SceneType::Lines;
            if (ImGui::Button("Triangles", ImVec2(-1, 0))) m_CurrentScene = SceneType::Triangles;
            if (ImGui::Button("Text", ImVec2(-1, 0))) m_CurrentScene = SceneType::Text;
            if (ImGui::Button("Pixels", ImVec2(-1, 0))) m_CurrentScene = SceneType::Pixels;
            if (ImGui::Button("Mixed", ImVec2(-1, 0))) m_CurrentScene = SceneType::Mixed;
            
            ImGui::End();

            ImGui::Begin("Camera");
            ImGui::SliderFloat2("Position", &m_CameraPosition.x, -20.0f, 20.0f);
            ImGui::SliderFloat("Zoom", &m_CameraZoom, 10.0f, 300.0f);
            if (ImGui::Button("Reset Camera")) 
            {
                m_CameraPosition = { 0.0f, 0.0f };
                m_CameraZoom = 25.0f;
            }
            ImGui::End();

            RenderSceneControls();

            ImGui::Begin("Global Lighting");
            ImGui::Checkbox("Enable Lighting", &m_LightingEnabled);
            ImGui::ColorEdit3("Ambient Color", &m_AmbientColor.x);
            ImGui::SliderFloat("Ambient Intensity", &m_AmbientIntensity, 0.0f, 1.0f);
            ImGui::End();

            ImGui::Begin("Quick Tests");

            float buttonWidth = ImGui::GetContentRegionAvail().x;

            if (ImGui::Button("Test All Blend Modes", ImVec2(buttonWidth, 0)))
            {
                CreateBlendModeTests();
            }
            if (ImGui::Button("Test All Falloff Types", ImVec2(buttonWidth, 0)))
            {
                CreateFalloffTests();
            }
            if (ImGui::Button("Animation Showcase", ImVec2(buttonWidth, 0)))
            {
                CreateAnimationTests();
            }

            ImGui::Separator();

            if (ImGui::Button("Clear All", ImVec2(buttonWidth, 0)))
            {
                m_Lights.clear();
            }
            if (ImGui::Button("Reset to Defaults", ImVec2(buttonWidth, 0)))
            {
                m_Lights.clear();
                AddTestLights();
            }

            ImGui::Separator();
            ImGui::Text("Total Lights: %d / 32", (int)m_Lights.size());
            ImGui::Text("Active Lights: %d", GetActiveLightCount());
            ImGui::End();

            ImGui::Begin("Light Controls", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar);
            for (size_t i = 0; i < m_Lights.size(); i++) 
            {
                auto& light = m_Lights[i];

                ImGui::PushID((int)i);

                bool headerOpen = ImGui::CollapsingHeader((light.name + " #" + std::to_string(i)).c_str());

                ImGui::SameLine();
                if (ImGui::SmallButton("Del")) 
                {
                    m_Lights.erase(m_Lights.begin() + i);
                    ImGui::PopID();
                    break;
                }

                if (headerOpen) {
                    ImGui::Checkbox("Enabled", &light.enabled);

                    if (light.enabled) 
                    {
                        ImGui::Text("Position & Animation");
                        ImGui::SliderFloat3("Position", &light.position.x, -15.0f, 15.0f);
                        ImGui::Checkbox("Animate Position", &light.animatePosition);

                        ImGui::Separator();

                        ImGui::Text("Color & Intensity");
                        ImGui::ColorEdit3("Color", &light.color.x);
                        ImGui::Checkbox("Animate Color", &light.animateColor);
                        ImGui::SliderFloat("Intensity", &light.intensity, 0.0f, 5.0f);
                        ImGui::Checkbox("Animate Intensity", &light.animateIntensity);
                        ImGui::SliderFloat("Radius", &light.radius, 1.0f, 20.0f);

                        if (light.animatePosition || light.animateIntensity || light.animateColor)
                        {
                            ImGui::SliderFloat("Animation Speed", &light.animationSpeed, 0.1f, 5.0f);
                        }

                        ImGui::Separator();

                        ImGui::Text("Blending");
                        const char* blendModes[] = 
                        {
                            "Additive", 
                            "Multiply", 
                            "Screen", 
                            "Overlay",
                            "SoftLight", 
                            "LinearBurn", 
                            "ColorDodge", 
                            "Subtract", 
                            "Alpha"
                        };
                        int currentBlend = (int)light.blendMode;
                        if (ImGui::Combo("Blend Mode", &currentBlend, blendModes, 9)) 
                        {
                            light.blendMode = (BlendMode)currentBlend;
                        }
                        ImGui::SliderFloat("Blend Alpha", &light.blendAlpha, 0.0f, 1.0f);

                        ImGui::Separator();

                        ImGui::Text("Attenuation");
                        const char* falloffTypes[] = 
                        {
                            "None", 
                            "Linear", 
                            "Quadratic", 
                            "InverseSquare",
                            "Exponential", 
                            "Smoothstep", 
                            "Custom", 
                            "Realistic"
                        };
                        int currentFalloff = (int)light.falloffType;
                        if (ImGui::Combo("Falloff Type", &currentFalloff, falloffTypes, 8)) 
                        {
                            light.falloffType = (AttenuationModel)currentFalloff;
                        }

                        const char* paramLabel = "Parameter";
                        if (light.falloffType == AttenuationModel::Exponential) paramLabel = "Decay Rate";
                        else if (light.falloffType == AttenuationModel::Custom) paramLabel = "Exponent";
                        else if (light.falloffType == AttenuationModel::Realistic) paramLabel = "Quadratic Term";

                        ImGui::SliderFloat(paramLabel, &light.falloffParam, 0.1f, 5.0f);

                        ImGui::Separator();

                        ImGui::Text("Presets");
                        if (ImGui::Button("Warm Light")) 
                        {
                            light.color = { 1.0f, 0.8f, 0.6f };
                            light.blendMode = BlendMode::Additive;
                            light.intensity = 1.5f;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Cool Shadow")) 
                        {
                            light.color = { 0.6f, 0.8f, 1.0f };
                            light.blendMode = BlendMode::Multiply;
                            light.intensity = 1.2f;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Neon")) 
                        {
                            light.color = { 1.0f, 0.2f, 1.0f };
                            light.blendMode = BlendMode::Screen;
                            light.intensity = 2.0f;
                        }
                    }
                }

                ImGui::Separator();
                ImGui::PopID();
            }

            if (ImGui::Button("Add New Light") && m_Lights.size() < 32) 
            {
                AddNewLight();
            }

            ImGui::End();

            ImGui::Begin("Debug Info");
            ImGui::Text("Camera Position: (%.2f, %.2f)", m_CameraPosition.x, m_CameraPosition.y);
            ImGui::Text("Current Scene: %s", sceneNames[(int)m_CurrentScene]);
            ImGui::Text("Lighting System: %s", m_LightingEnabled ? "Enabled" : "Disabled");

            if (ImGui::CollapsingHeader("Light Summary")) 
            {
                for (size_t i = 0; i < m_Lights.size(); i++) 
                {
                    const auto& light = m_Lights[i];
                    ImGui::Text("%s: %s, Blend: %d, Falloff: %d",
                        light.name.c_str(),
                        light.enabled ? "ON" : "OFF",
                        (int)light.blendMode,
                        (int)light.falloffType
                    );
                }
            }
            ImGui::End();
        }

        void RenderSceneControls()
        {
            ImGui::Begin("Scene Controls");

            switch (m_CurrentScene)
            {
            case SceneType::QuadGrid:
                ImGui::Text("Quad Grid Settings (Original Scene)");
                ImGui::SliderInt("Grid Width", &m_GridWidth, 3, 15);
                ImGui::SliderInt("Grid Height", &m_GridHeight, 3, 15);
                ImGui::SliderFloat("Spacing", &m_QuadSpacing, 2.0f, 8.0f);
                ImGui::SliderFloat2("Quad Size", &m_QuadSize.x, 0.5f, 4.0f);
				ImGui::DragFloat3("Quad Position Offset", &m_QuadPositionOffset.x);
                ImGui::ColorEdit4("Quad Color", &m_QuadColor.x);
                break;

            case SceneType::Circles:
                ImGui::Text("Circle Settings");
                ImGui::SliderInt("Grid Width", &m_GridWidth, 3, 15);
                ImGui::SliderInt("Grid Height", &m_GridHeight, 3, 15);
                ImGui::SliderFloat("Spacing", &m_QuadSpacing, 2.0f, 8.0f);
                ImGui::SliderFloat2("Radius", &m_CircleRadius.x, 0.5f, 3.0f);
                ImGui::SliderFloat("Thickness", &m_CircleThickness, 0.1f, 1.0f);
                ImGui::SliderFloat("Fade", &m_CircleFade, 0.0f, 0.5f);
                ImGui::ColorEdit4("Circle Color", &m_CircleColor.x);
                break;

            case SceneType::Lines:
                ImGui::Text("Line Settings");
                ImGui::SliderInt("Line Count", &m_LineCount, 6, 24);
                ImGui::SliderFloat("Length", &m_LineLength, 4.0f, 15.0f);
                ImGui::SliderFloat("Thickness", &m_LineThickness, 1.0f, 8.0f);
                ImGui::ColorEdit4("Line Color", &m_LineColor.x);
                break;

            case SceneType::Triangles:
                ImGui::Text("Triangle Settings");
                ImGui::SliderFloat("Size", &m_TriangleSize, 1.0f, 4.0f);
                ImGui::ColorEdit4("Triangle Color", &m_TriangleColor.x);
                break;

            case SceneType::Text:
            {
                ImGui::Text("Text Settings");
                char buffer[256] = {};
                size_t len = std::min(m_TextContent.length(), sizeof(buffer) - 1);
                m_TextContent.copy(buffer, len);
                if (ImGui::InputText("Content", buffer, sizeof(buffer))) 
                {
                    m_TextContent = std::string(buffer); 
                }
                ImGui::SliderFloat("Size", &m_TextSize, 0.5f, 4.0f);
                ImGui::ColorEdit4("Text Color", &m_TextColor.x);
                break;
            }

            case SceneType::Pixels:
            {
                ImGui::Text("Pixel Settings");
                ImGui::SliderInt("Density", &m_PixelDensity, 10, 50);
                ImGui::SliderFloat("Size", &m_PixelSize, 1.0f, 8.0f);
                ImGui::ColorEdit4("Pixel Color", &m_PixelColor.x);
                const char* patterns[] = { "Circles", "Checkerboard", "Cross", "Random" };
                ImGui::Combo("Pattern", &m_PixelPattern, patterns, 4);
                break;
            }

            case SceneType::Mixed:
                ImGui::Text("Mixed Scene - All shapes combined");
                ImGui::Text("No specific controls for this scene");
                break;
            }

            ImGui::End();
        }

        void CreateBlendModeTests()
        {
            m_Lights.clear();

            const char* names[] = 
            { 
                "Additive", 
                "Multiply", 
                "Screen", 
                "Overlay", 
                "SoftLight", 
                "LinearBurn", 
                "ColorDodge", 
                "Subtract", 
                "Alpha" 
            };
            
            glm::vec3 colors[] = 
            {
                {1.0f, 0.3f, 0.3f}, 
                {0.3f, 0.3f, 1.0f}, 
                {1.0f, 1.0f, 0.3f}, 
                {0.3f, 1.0f, 0.3f},
                {1.0f, 0.6f, 0.3f}, 
                {0.6f, 0.3f, 1.0f}, 
                {1.0f, 0.3f, 1.0f}, 
                {0.8f, 0.8f, 0.8f}, 
                {0.5f, 0.8f, 1.0f}
            };

            for (int i = 0; i < 9; i++) 
            {
                LightData light;
                light.name = names[i];
                light.position = { (i % 3 - 1) * 8.0f, (i / 3 - 1) * 6.0f, 0.2f };
                light.color = colors[i];
                light.intensity = 1.5f;
                light.radius = 6.0f;
                light.blendMode = (BlendMode)i;
                light.blendAlpha = 0.8f;
                light.falloffType = AttenuationModel::Quadratic;
                m_Lights.push_back(light);
            }
        }

        void CreateFalloffTests()
        {
            m_Lights.clear();

            const char* names[] = 
            { 
                "None", 
                "Linear", 
                "Quadratic", 
                "InverseSquare", 
                "Exponential", 
                "Smoothstep", 
                "Custom", 
                "Realistic" 
            };

            for (int i = 0; i < 8; i++) 
            {
                LightData light;
                light.name = names[i];
                light.position = { (i % 4 - 1.5f) * 8.0f, (i / 4 - 0.5f) * 8.0f, 0.2f };
                light.color = { 1.0f, 0.7f, 0.3f };
                light.intensity = 1.5f;
                light.radius = 8.0f;
                light.blendMode = BlendMode::Additive;
                light.falloffType = (AttenuationModel)i;
                light.falloffParam = (i == 4) ? 0.3f : 1.0f;
                m_Lights.push_back(light);
            }
        }

        void CreateAlphaTests()
        {
            m_Lights.clear();

            float alphas[] = { 0.2f, 0.4f, 0.6f, 0.8f, 1.0f };

            for (int i = 0; i < 5; i++) 
            {
                LightData light;
                light.name = "Alpha " + std::to_string((int)(alphas[i] * 100)) + "%";
                light.position = { (i - 2) * 6.0f, 0.0f, 0.2f };
                light.color = { 1.0f, 0.5f, 0.8f };
                light.intensity = 2.0f;
                light.radius = 8.0f;
                light.blendMode = BlendMode::Additive;
                light.blendAlpha = alphas[i];
                light.falloffType = AttenuationModel::Quadratic;
                m_Lights.push_back(light);
            }
        }

        void CreateAnimationTests()
        {
            m_Lights.clear();

            // Orbiting light
            LightData orbiting;
            orbiting.name = "Orbiting";
            orbiting.color = { 1.0f, 0.3f, 0.3f };
            orbiting.intensity = 2.0f;
            orbiting.radius = 6.0f;
            orbiting.animatePosition = true;
            orbiting.animationSpeed = 1.0f;
            m_Lights.push_back(orbiting);

            // Pulsing light
            LightData pulsing;
            pulsing.name = "Pulsing";
            pulsing.position = { 8.0f, 0.0f, 0.2f };
            pulsing.color = { 0.3f, 1.0f, 0.3f };
            pulsing.animateIntensity = true;
            pulsing.animationSpeed = 2.0f;
            m_Lights.push_back(pulsing);

            // Color changing light
            LightData colorful;
            colorful.name = "Rainbow";
            colorful.position = { -8.0f, 0.0f, 0.2f };
            colorful.intensity = 1.5f;
            colorful.radius = 8.0f;
            colorful.animateColor = true;
            colorful.animationSpeed = 1.5f;
            m_Lights.push_back(colorful);
        }

        void AddNewLight()
        {
            LightData newLight;
            newLight.name = "Custom Light " + std::to_string(m_Lights.size());
            newLight.position = { 0.0f, 0.0f, 0.2f };
            newLight.color = { 1.0f, 1.0f, 1.0f };
            newLight.intensity = 1.0f;
            newLight.radius = 5.0f;
            newLight.blendMode = BlendMode::Additive;
            newLight.blendAlpha = 1.0f;
            newLight.falloffType = AttenuationModel::Quadratic;
            newLight.falloffParam = 1.0f;
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

        Ref<OrthographicCamera> m_Camera;
        glm::vec2 m_CameraPosition = { 0.0f, 0.0f };
        float m_CameraZoom = 25.0f;

        // Scene management
        SceneType m_CurrentScene = SceneType::QuadGrid;

        int m_GridWidth = 8;
        int m_GridHeight = 8;
        float m_QuadSpacing = 4.0f;
        glm::vec2 m_QuadSize = { 4.0f, 4.0f };
        glm::vec3 m_QuadPositionOffset = { 0.0f, 0.0f, 0.0f };
        glm::vec4 m_QuadColor = { 0.9f, 0.9f, 0.9f, 1.0f };  // Soft light gray instead of harsh white

        // Circle settings - warm orange/coral theme
        glm::vec2 m_CircleRadius = { 4.0f, 4.0f };
        glm::vec4 m_CircleColor = { 1.0f, 0.6f, 0.4f, 0.9f };  // Warm coral with slight transparency
        float m_CircleThickness = 0.8f;  // Thicker for better visibility
        float m_CircleFade = 0.05f;  // Subtle fade, not harsh

        // Line settings - cool blue theme
        glm::vec4 m_LineColor = { 0.4f, 0.7f, 1.0f, 0.8f };  // Softer blue with transparency
        float m_LineThickness = 2.5f;  // Slightly thinner for elegance
        int m_LineCount = 16;  // More lines for fuller pattern
        float m_LineLength = 8.0f;

        // Triangle settings - mint green theme
        glm::vec4 m_TriangleColor = { 0.3f, 0.9f, 0.7f, 0.85f };  // Mint green with transparency
        float m_TriangleSize = 2.0f;

        // Text settings - golden yellow theme
        std::string m_TextContent = "LUMINA";
        glm::vec4 m_TextColor = { 1.0f, 0.8f, 0.2f, 1.0f };  // Golden yellow, more readable
        float m_TextSize = 2.0f;

        // Pixel settings - vibrant purple theme
        glm::vec4 m_PixelColor = { 0.7f, 0.3f, 1.0f, 0.9f };  // Vibrant purple with slight transparency
        float m_PixelSize = 3.0f;  // Smaller for better detail
        int m_PixelDensity = 25;  // Higher density for richer patterns
        int m_PixelPattern = 0;

        // Lighting settings - warmer ambient for better mood
        bool m_LightingEnabled = true;
        glm::vec3 m_AmbientColor = { 0.7f, 0.75f, 0.8f };  // Slightly cool-tinted ambient
        float m_AmbientIntensity = 0.6f;  // Lower for more dramatic lighting contrast

        // Light management
        std::vector<LightData> m_Lights;
};
}