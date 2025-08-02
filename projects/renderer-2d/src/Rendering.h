#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>

#include "imgui.h"

#include "Lumina/Lumina.h"

#include <glm/glm.hpp>

namespace Lumina
{
    class Rendering : public Layer
    {
    public:
        enum class SceneCamera
        {
            Perspective = 0,
            Orthographic = 1
        };

        virtual void OnAttach() override
        {
            Renderer2D::Init();

            std::string source = "res/texture/factory_atlas.png";
            m_Atlas = TextureAtlas::Create(source, 16, 16);

            // Initialize quad with texture enabled by default
            m_Quad.Texture = m_Atlas->GetTexture();
            m_Quad.TextureCoords = m_Atlas->GetTextureCoords(m_QuadTextureIndex);
            m_Quad.Position = { -0.5f, -0.5f, 0.0f };

            // Initialize circle with texture enabled by default
            m_Circle.Texture = m_Atlas->GetTexture();
            m_Circle.TextureCoords = m_Atlas->GetTextureCoords(m_CircleTextureIndex);
            m_Circle.Position = { 0.5f, -0.5f, 0.0f };

            // Initialize line
            m_Line.Start = { -0.5f, 0.5f, 0.0f };
            m_Line.End = { 0.5f, 0.5f, 0.0f };

            // Setup cameras
            m_PerspectiveCamera.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
            m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

            m_OrthographicCamera.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
            m_OrthographicCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            m_OrthographicCamera.SetZoom(4.0f);
        }

        virtual void OnDetach() override
        {
            Renderer2D::Shutdown();
        }

        virtual void OnUpdate(float ts) override
        {
            Renderer2D::ResetStats();
        }

        virtual void OnUIRender() override
        {
            // Main scene viewer
            ImGui::Begin("Scene Viewer");
            ImGui::SetCursorPos({ 0.0f, 0.0f });

            ImVec2 size = ImGui::GetContentRegionAvail();

            m_PerspectiveCamera.SetAspectRatio(size.x / size.y);
            Renderer2D::SetResolution(size.x, size.y);

            Renderer2D::SetRenderMode(m_Mode);

            if (m_SceneCamera == SceneCamera::Perspective)
                Renderer2D::Begin(m_PerspectiveCamera);
            else
                Renderer2D::Begin(m_OrthographicCamera);

            Renderer2D::DrawQuad(m_Quad);
            Renderer2D::DrawCircle(m_Circle);
            Renderer2D::DrawLine(m_Line);

            Renderer2D::End();

            ImGui::Image((void*)(intptr_t)Renderer2D::GetImage(), { size.x, size.y });
            ImGui::End();

            // Quad Controls
            ImGui::Begin("Quad Controls");
            ImGui::DragFloat3("Position", glm::value_ptr(m_Quad.Position), 0.01f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(m_Quad.Rotation), 0.01f);
            ImGui::DragFloat2("Size", glm::value_ptr(m_Quad.Size), 0.01f, 0.01f, 10.0f);
            ImGui::ColorEdit4("Tint Color", glm::value_ptr(m_Quad.TintColor));

            // Texture controls for quad
            ImGui::Separator();
            ImGui::Text("Texture Settings");
            ImGui::Checkbox("Enable Texture##Quad", &m_QuadTextureEnabled);

            if (m_QuadTextureEnabled)
            {
                int maxIndex = std::max(0, m_Atlas->GetWidth() * m_Atlas->GetHeight() - 1);
                if (ImGui::DragInt("Texture Index##Quad", &m_QuadTextureIndex, 1.0f, 0, maxIndex))
                {
                    m_QuadTextureIndex = std::clamp(m_QuadTextureIndex, 0, maxIndex);
                    m_Quad.TextureCoords = m_Atlas->GetTextureCoords(m_QuadTextureIndex);
                }
                ImGui::DragFloat4("Texture Coords##Quad", glm::value_ptr(m_Quad.TextureCoords), 0.01f, 0.0f, 1.0f);
                m_Quad.Texture = m_Atlas->GetTexture();
            }
            else
            {
                m_Quad.Texture = nullptr;
            }
            ImGui::End();

            // Circle Controls
            ImGui::Begin("Circle Controls");
            ImGui::DragFloat3("Position", glm::value_ptr(m_Circle.Position), 0.01f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(m_Circle.Rotation), 0.01f);
            ImGui::DragFloat2("Radius", glm::value_ptr(m_Circle.Radius), 0.01f, 0.01f, 10.0f);
            ImGui::ColorEdit4("Color", glm::value_ptr(m_Circle.Color));
            ImGui::DragFloat("Thickness", &m_Circle.Thickness, 0.01f, 0.01f, 10.0f);
            ImGui::DragFloat("Fade", &m_Circle.Fade, 0.01f, 0.0f, 1.0f);

            // Texture controls for circle
            ImGui::Separator();
            ImGui::Text("Texture Settings");
            ImGui::Checkbox("Enable Texture##Circle", &m_CircleTextureEnabled);

            if (m_CircleTextureEnabled)
            {
                int maxIndex = std::max(0, m_Atlas->GetWidth() * m_Atlas->GetHeight() - 1);
                if (ImGui::DragInt("Texture Index##Circle", &m_CircleTextureIndex, 1.0f, 0, maxIndex))
                {
                    m_CircleTextureIndex = std::clamp(m_CircleTextureIndex, 0, maxIndex);
                    m_Circle.TextureCoords = m_Atlas->GetTextureCoords(m_CircleTextureIndex);
                }
                ImGui::DragFloat4("Texture Coords##Circle", glm::value_ptr(m_Circle.TextureCoords), 0.01f, 0.0f, 1.0f);
                m_Circle.Texture = m_Atlas->GetTexture();
            }
            else
            {
                m_Circle.Texture = nullptr;
            }
            ImGui::End();

            // Line Controls
            ImGui::Begin("Line Controls");
            ImGui::DragFloat3("Start Position", glm::value_ptr(m_Line.Start), 0.01f);
            ImGui::DragFloat3("End Position", glm::value_ptr(m_Line.End), 0.01f);
            ImGui::DragFloat("Thickness", &m_Line.Thickness, 0.1f, 0.1f, 20.0f);
            ImGui::ColorEdit4("Color", glm::value_ptr(m_Line.Color));
            ImGui::End();

            // Scene Controls
            ImGui::Begin("Scene Controls");
            static int cameraMode = 0;
            const char* items[] = { "Perspective", "Orthographic" };
            if (ImGui::Combo("Camera Mode", &cameraMode, items, IM_ARRAYSIZE(items)))
                m_SceneCamera = static_cast<SceneCamera>(cameraMode);
            ImGui::End();

            ImGui::Begin("Draw Controls");
            if (ImGui::BeginCombo("Polygon Mode",
                m_Mode == PolygonMode::Fill ? "Fill" :
                m_Mode == PolygonMode::Line ? "Line" : "Point"))
            {
                if (ImGui::Selectable("Fill", m_Mode == PolygonMode::Fill))
                    m_Mode = PolygonMode::Fill;

                if (ImGui::Selectable("Line", m_Mode == PolygonMode::Line))
                    m_Mode = PolygonMode::Line;

                if (ImGui::Selectable("Point", m_Mode == PolygonMode::Point))
                    m_Mode = PolygonMode::Point;

                ImGui::EndCombo();
            }
            ImGui::End(); 

            // Camera Controls
            {
                ImGui::Begin("Perspective Camera");
                glm::vec3 position = m_PerspectiveCamera.GetPosition();
                glm::quat rotation = m_PerspectiveCamera.GetRotation();

                bool updated = false;
                updated |= ImGui::DragFloat3("Position", glm::value_ptr(position), 0.01f);
                updated |= ImGui::DragFloat4("Rotation", glm::value_ptr(rotation), 0.01f);

                if (updated)
                {
                    m_PerspectiveCamera.SetPosition(position);
                    // m_PerspectiveCamera.SetRotation(rotation);
                }
                ImGui::End();
            }

            {
                ImGui::Begin("Orthographic Camera");
                glm::vec3 position = m_OrthographicCamera.GetPosition();
                glm::quat rotation = m_OrthographicCamera.GetRotation();
                float zoom = m_OrthographicCamera.GetZoom();

                bool updated = false;
                updated |= ImGui::DragFloat3("Position", glm::value_ptr(position), 0.01f);
                updated |= ImGui::DragFloat4("Rotation", glm::value_ptr(rotation), 0.01f);
                updated |= ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.1f, 10.0f);

                if (updated)
                {
                    m_OrthographicCamera.SetPosition(position);
                    // m_OrthographicCamera.SetRotation(rotation);
                    m_OrthographicCamera.SetZoom(zoom);
                }
                ImGui::End();
            }

            // Render Statistics
            ImGui::Begin("Render Statistics");
            Renderer2D::Statistics stats = Renderer2D::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quad Count: %d", stats.QuadCount);
            ImGui::Text("Circle Count: %d", stats.CircleCount);
            ImGui::Text("Line Count: %d", stats.LineCount);
            ImGui::Text("Shaders Used: %d", stats.ShadersUsed);
            ImGui::Text("Textures Used: %d", stats.TexturesUsed);
            ImGui::Text("Data Size (Bytes): %d", stats.DataSize);
            ImGui::End();

            // Texture Atlas Preview
            ImGui::Begin("Texture Atlas");
            ImGui::Image((void*)m_Atlas->GetTexture()->GetID(), { 200, 200 });
            ImGui::End();
        }

    private:
        QuadAttributes m_Quad;
        CircleAttributes m_Circle;
        LineAttributes m_Line;

        PerspectiveCamera m_PerspectiveCamera;
        OrthographicCamera m_OrthographicCamera;
        SceneCamera m_SceneCamera = SceneCamera::Perspective;

        Ref<TextureAtlas> m_Atlas;
        Timer m_FrameTimer;
        float m_FPS = 0.0f;

        PolygonMode m_Mode = PolygonMode::Fill; 

        // Texture control variables
        int m_QuadTextureIndex = 121;
        int m_CircleTextureIndex = 160;
        bool m_QuadTextureEnabled = true;
        bool m_CircleTextureEnabled = true;
    };
}