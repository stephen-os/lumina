#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>

#include "imgui.h"

#include "Lumina/Core/Layer.h"
#include "Lumina/Utils/Timer.h"
#include "Lumina/Graphics/Renderer.h"
#include "Lumina/Graphics/TextureAtlas.h"

#include "Lumina/Graphics/Cameras/OrthographicCamera.h"
#include "Lumina/Graphics/Cameras/PerspectiveCamera.h"

#include <glm/glm.hpp>

namespace Lumina
{
    class AdvancedRendering : public Layer
    {
    public:
        enum class SceneCamera
        {
            Perspective = 0,
            Orthographic = 1
        };

        virtual void OnAttach() override
        {
            Renderer::Init();

            std::string source = "res/texture/factory_atlas.png";
            m_Atlas = TextureAtlas::Create(source, 16, 16);

            m_Cube.Texture = m_Atlas->GetTexture();
            m_Cube.TextureCoords = m_Atlas->GetTextureCoords(m_TextureIndex);

            m_PerspectiveCamera.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
            m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

            m_OrthographicCamera.SetPosition(glm::vec3(0.0f, 0.0f, 10.0f));
            m_OrthographicCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            m_OrthographicCamera.SetZoom(4.0f);
        }

        virtual void OnDetach() override
        {
            Renderer::Shutdown();
        }

        virtual void OnUpdate(float ts) override
        {
            Renderer::ResetStats();

            m_RotationTime += ts;

            if (m_Animate)
            {
                m_Cube.Rotation.x = glm::radians(45.0f) * sin(m_RotationTime);
                m_Cube.Rotation.y = glm::radians(90.0f) * cos(m_RotationTime * 0.8f);
                m_Cube.Rotation.z = glm::radians(60.0f) * sin(m_RotationTime * 1.2f);
            }
        }

        virtual void OnUIRender() override
        {
            ImGui::Begin("Scene Viewer");
            ImGui::SetCursorPos({ 0.0f, 0.0f });

            ImVec2 size = ImGui::GetContentRegionAvail();

            m_PerspectiveCamera.SetAspectRatio(size.x / size.y);
            Renderer::SetResolution(size.x, size.y);
            
            if (m_SceneCamera == SceneCamera::Perspective)
                Renderer::Begin(m_PerspectiveCamera);
            else
                Renderer::Begin(m_OrthographicCamera);

            Renderer::DrawCube(m_Cube);

            Renderer::End();

            ImGui::Image((void*)(intptr_t)Renderer::GetImage(), { size.x, size.y });
            ImGui::End();

            ImGui::Begin("Scene Controls");

            static int cameraMode = 0;
            const char* items[] = { "Perspective", "Orthographic" };
            if (ImGui::Combo("Camera Mode", &cameraMode, items, IM_ARRAYSIZE(items)))
                m_SceneCamera = static_cast<SceneCamera>(cameraMode);

            ImGui::Checkbox("Animate Rotation", &m_Animate);
            ImGui::DragFloat3("Cube Position", glm::value_ptr(m_Cube.Position), 0.01f);
            ImGui::DragFloat3("Cube Rotation", glm::value_ptr(m_Cube.Rotation), 0.01f);
            ImGui::DragFloat3("Cube Size", glm::value_ptr(m_Cube.Size), 0.01f);

            int maxIndex = std::max(0, m_Atlas->GetWidth() * m_Atlas->GetHeight() - 1);
            if (ImGui::DragInt("Texture Index", &m_TextureIndex, 1.0f, 0, maxIndex))
            {
				m_TextureIndex = std::clamp(m_TextureIndex, 0, maxIndex);
                m_Cube.TextureCoords = m_Atlas->GetTextureCoords(m_TextureIndex);
                m_Cube.Texture = m_Atlas->GetTexture();
            }

            ImGui::ColorEdit4("Tint Color", glm::value_ptr(m_Cube.TintColor));

            if (ImGui::Button("Reset Rotation"))
            {
                m_RotationTime = 0.0f;
                m_Animate = false;
                m_Cube.Rotation = { 0.0f, 0.0f, 0.0f };
            }
            ImGui::End();

            ImGui::Begin("Render Statistic");
            Renderer::Statistics stats = Renderer::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Quad Count: %d", stats.QuadCount);
            ImGui::Text("Shaders Used: %d", stats.ShadersUsed);
            ImGui::Text("Textures Used: %d", stats.TexturesUsed);
            ImGui::Text("Data Size (Bytes): %d", stats.DataSize);
            ImGui::End();

            ImGui::Begin("Texture Atlas");
            ImGui::Image((void*)m_Atlas->GetTexture()->GetID(), { 200, 200 });
            ImGui::End();

            {
                ImGui::Begin("Perspective Controls");
                glm::vec3 position = m_PerspectiveCamera.GetPosition();
                glm::quat rotation = m_PerspectiveCamera.GetRotation();

                bool updated = false;
                updated |= ImGui::DragFloat3("Camera Position", glm::value_ptr(position), 0.01f);
                updated |= ImGui::DragFloat4("Camera Rotation", glm::value_ptr(rotation), 0.01f);

                if (updated)
                {
                    m_PerspectiveCamera.SetPosition(position);
                    m_PerspectiveCamera.SetRotation(rotation);
                }
                ImGui::End();
            }

            {
                ImGui::Begin("Orthographic Controls");
                glm::vec3 position = m_OrthographicCamera.GetPosition();
                glm::quat rotation = m_OrthographicCamera.GetRotation();
                float zoom = m_OrthographicCamera.GetZoom();

                bool updated = false;
                updated |= ImGui::DragFloat3("Camera Position", glm::value_ptr(position), 0.01f);
                updated |= ImGui::DragFloat4("Camera Rotation", glm::value_ptr(rotation), 0.01f);
                updated |= ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.1f, 10.0f);

                if (updated)
                {
                    m_OrthographicCamera.SetPosition(position);
                    m_OrthographicCamera.SetRotation(rotation);
                    m_OrthographicCamera.SetZoom(zoom);
                }
                ImGui::End();
            }
        }

    private:
        CubeAttributes m_Cube;

        PerspectiveCamera m_PerspectiveCamera;
        OrthographicCamera m_OrthographicCamera;
        SceneCamera m_SceneCamera = SceneCamera::Perspective;

        Ref<TextureAtlas> m_Atlas;
        Timer m_FrameTimer;
        float m_FPS = 0.0f;

        // Cube Controls
        float m_RotationTime = 0.0f;
		bool m_Animate = true;
		int m_TextureIndex = 118;
    };
}
