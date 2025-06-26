#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>

#include "imgui.h"

#include "Lumina/Core/Layer.h"
#include "Lumina/Utils/Timer.h"
#include "Lumina/Graphics/Renderer3D.h"
#include "Lumina/Graphics/Model.h"

#include "Lumina/Graphics/Cameras/OrthographicCamera.h"
#include "Lumina/Graphics/Cameras/PerspectiveCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
            // Initialize 3D renderer
            Renderer3D::Init();

            // Try to load cube model from file first
            m_CubeModel = Model::Create("res/geometry/suzzan.gltf"); 

            if (!m_CubeModel || m_CubeModel->GetMeshes().empty())
            {
                std::cout << "Could not load cube.gltf, creating procedural cube..." << std::endl;
                // Create a procedural cube if file loading fails
                // CreateProceduralCube();
            }

            // Initialize model attributes
            m_ModelAttribs.Position = { 0.0f, 0.0f, 0.0f };
            m_ModelAttribs.Rotation = { 0.0f, 0.0f, 0.0f };
            m_ModelAttribs.Scale = { 1.0f, 1.0f, 1.0f };
            m_ModelAttribs.TintColor = { 1.0f, 1.0f, 1.0f, 1.0f };

            // Setup cameras
            m_PerspectiveCamera.SetPosition(glm::vec3(3.0f, 3.0f, 5.0f));
            m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            m_PerspectiveCamera.SetFOV(45.0f);
			m_PerspectiveCamera.SetClippingPlanes(0.1f, 100.0f);

            m_OrthographicCamera.SetPosition(glm::vec3(3.0f, 3.0f, 5.0f));
            m_OrthographicCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            m_OrthographicCamera.SetZoom(2.0f);

            // Setup lighting
            m_DirectionalLight.Direction = glm::normalize(glm::vec3(-0.2f, -1.0f, -0.3f));
            m_DirectionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
            m_DirectionalLight.Intensity = 3.0f;
            Renderer3D::SetDirectionalLight(m_DirectionalLight);

            // Add some point lights
            PointLight pointLight1;
            pointLight1.Position = glm::vec3(2.0f, 2.0f, 2.0f);
            pointLight1.Color = glm::vec3(1.0f, 0.2f, 0.2f);
            pointLight1.Intensity = 5.0f;
            pointLight1.Constant = 1.0f;
            pointLight1.Linear = 0.09f;
            pointLight1.Quadratic = 0.032f;
            Renderer3D::AddPointLight(pointLight1);

            PointLight pointLight2;
            pointLight2.Position = glm::vec3(-2.0f, 2.0f, 2.0f);
            pointLight2.Color = glm::vec3(0.2f, 1.0f, 0.2f);
            pointLight2.Intensity = 5.0f;
            pointLight2.Constant = 1.0f;
            pointLight2.Linear = 0.09f;
            pointLight2.Quadratic = 0.032f;
            Renderer3D::AddPointLight(pointLight2);

            // Setup default point lights for UI
            m_PointLights.push_back(pointLight1);
            m_PointLights.push_back(pointLight2);
        }

        virtual void OnDetach() override
        {
            Renderer3D::Shutdown();
        }

        virtual void OnUpdate(float ts) override
        {
            Renderer3D::ResetStats();

            m_FPS = 1.0f / m_FrameTimer.Elapsed();
            m_FrameTimer.Reset();

            // Auto-rotate the cube if enabled
            if (m_AutoRotate)
            {
                m_ModelAttribs.Rotation.y += ts * m_RotationSpeed;
                if (m_ModelAttribs.Rotation.y > 360.0f)
                    m_ModelAttribs.Rotation.y -= 360.0f;
            }
        }

        virtual void OnUIRender() override
        {
            ImGui::Begin("FPS");
            ImGui::Text("FPS: %.2f", m_FPS);
            ImGui::End(); 

            // Main 3D scene viewer
            ImGui::Begin("3D Scene Viewer");
            ImGui::SetCursorPos({ 0.0f, 0.0f });

            ImVec2 size = ImGui::GetContentRegionAvail();

            // Update camera aspect ratio and renderer resolution
            m_PerspectiveCamera.SetAspectRatio(size.x / size.y);
            Renderer3D::SetResolution(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y));

            // Set render mode
            Renderer3D::SetRenderMode(m_Mode);

            // Begin rendering
            if (m_SceneCamera == SceneCamera::Perspective)
                Renderer3D::Begin(m_PerspectiveCamera);
            else
                Renderer3D::Begin(m_OrthographicCamera);

            // Draw the cube model
            if (m_CubeModel && !m_CubeModel->GetMeshes().empty())
            {
                Renderer3D::DrawModel(m_CubeModel, m_ModelAttribs);
            }

            // End rendering
            Renderer3D::End();

            // Display the rendered image
            ImGui::Image((void*)(intptr_t)Renderer3D::GetImage(), { size.x, size.y });
            ImGui::End();

            // Model Controls
            ImGui::Begin("Cube Model Controls");
            ImGui::DragFloat3("Position", glm::value_ptr(m_ModelAttribs.Position), 0.01f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(m_ModelAttribs.Rotation), 0.01f);
            ImGui::DragFloat3("Scale", glm::value_ptr(m_ModelAttribs.Scale), 0.01f, 0.01f, 10.0f);
            ImGui::ColorEdit4("Tint Color", glm::value_ptr(m_ModelAttribs.TintColor));

            ImGui::Separator();
            ImGui::Checkbox("Auto Rotate", &m_AutoRotate);
            if (m_AutoRotate)
            {
                ImGui::DragFloat("Rotation Speed", &m_RotationSpeed, 0.1f, 0.0f, 10.0f);
            }
            ImGui::End();

            // Scene Controls
            ImGui::Begin("Scene Controls");
            static int cameraMode = 0;
            const char* items[] = { "Perspective", "Orthographic" };
            if (ImGui::Combo("Camera Mode", &cameraMode, items, IM_ARRAYSIZE(items)))
                m_SceneCamera = static_cast<SceneCamera>(cameraMode);
            ImGui::End();

            // Render Mode Controls
            ImGui::Begin("Render Controls");
            if (ImGui::BeginCombo("Render Mode",
                m_Mode == RenderMode::Normal ? "Normal" :
                m_Mode == RenderMode::Wireframe ? "Wireframe" : "Points"))
            {
                if (ImGui::Selectable("Normal", m_Mode == RenderMode::Normal))
                    m_Mode = RenderMode::Normal;

                if (ImGui::Selectable("Wireframe", m_Mode == RenderMode::Wireframe))
                    m_Mode = RenderMode::Wireframe;

                if (ImGui::Selectable("Points", m_Mode == RenderMode::Points))
                    m_Mode = RenderMode::Points;

                ImGui::EndCombo();
            }
            ImGui::End();

            // Camera Controls
            {
                ImGui::Begin("Perspective Camera");
                glm::vec3 position = m_PerspectiveCamera.GetPosition();
                glm::quat rotation = m_PerspectiveCamera.GetRotation();
                float fov = m_PerspectiveCamera.GetFOV();
                float nearPlane = m_PerspectiveCamera.GetNearPlane();
                float farPlane = m_PerspectiveCamera.GetFarPlane();

                bool updated = false;
                updated |= ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
                updated |= ImGui::DragFloat4("Rotation", glm::value_ptr(rotation), 0.01f);
                updated |= ImGui::DragFloat("FOV", &fov, 1.0f, 1.0f, 179.0f);
                updated |= ImGui::DragFloat("Near Plane", &nearPlane, 0.01f, 0.01f, 10.0f);
                updated |= ImGui::DragFloat("Far Plane", &farPlane, 1.0f, 1.0f, 1000.0f);

                if (updated)
                {
                    m_PerspectiveCamera.SetPosition(position);
                    m_PerspectiveCamera.SetRotation(rotation);
                    m_PerspectiveCamera.SetFOV(fov);
					m_PerspectiveCamera.SetClippingPlanes(nearPlane, farPlane);
                }

                if (ImGui::Button("Reset Camera"))
                {
                    m_PerspectiveCamera.SetPosition(glm::vec3(3.0f, 3.0f, 5.0f));
                    m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }
                ImGui::End();
            }

            {
                ImGui::Begin("Orthographic Camera");
                glm::vec3 position = m_OrthographicCamera.GetPosition();
                glm::quat rotation = m_OrthographicCamera.GetRotation();
                float zoom = m_OrthographicCamera.GetZoom();

                bool updated = false;
                updated |= ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
                updated |= ImGui::DragFloat4("Rotation", glm::value_ptr(rotation), 0.01f);
                updated |= ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.1f, 10.0f);

                if (updated)
                {
                    m_OrthographicCamera.SetPosition(position);
                    m_OrthographicCamera.SetRotation(rotation);
                    m_OrthographicCamera.SetZoom(zoom);
                }

                if (ImGui::Button("Reset Camera"))
                {
                    m_OrthographicCamera.SetPosition(glm::vec3(3.0f, 3.0f, 5.0f));
                    m_OrthographicCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                    m_OrthographicCamera.SetZoom(2.0f);
                }
                ImGui::End();
            }

            // Lighting Controls
            ImGui::Begin("Lighting Controls");

            // Directional Light
            ImGui::Text("Directional Light");
            ImGui::Separator();
            bool lightUpdated = false;
            lightUpdated |= ImGui::DragFloat3("Direction", glm::value_ptr(m_DirectionalLight.Direction), 0.01f);
            lightUpdated |= ImGui::ColorEdit3("Color", glm::value_ptr(m_DirectionalLight.Color));
            lightUpdated |= ImGui::DragFloat("Intensity", &m_DirectionalLight.Intensity, 0.1f, 0.0f, 10.0f);

            if (lightUpdated)
            {
                m_DirectionalLight.Direction = glm::normalize(m_DirectionalLight.Direction);
                Renderer3D::SetDirectionalLight(m_DirectionalLight);
            }

            ImGui::Spacing();
            ImGui::Text("Point Lights");
            ImGui::Separator();

            // Point Lights
            Renderer3D::ClearPointLights();
            for (size_t i = 0; i < m_PointLights.size(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));

                if (ImGui::CollapsingHeader(("Point Light " + std::to_string(i + 1)).c_str()))
                {
                    ImGui::DragFloat3("Position", glm::value_ptr(m_PointLights[i].Position), 0.1f);
                    ImGui::ColorEdit3("Color", glm::value_ptr(m_PointLights[i].Color));
                    ImGui::DragFloat("Intensity", &m_PointLights[i].Intensity, 0.1f, 0.0f, 20.0f);
                    ImGui::DragFloat("Constant", &m_PointLights[i].Constant, 0.01f, 0.0f, 2.0f);
                    ImGui::DragFloat("Linear", &m_PointLights[i].Linear, 0.001f, 0.0f, 1.0f);
                    ImGui::DragFloat("Quadratic", &m_PointLights[i].Quadratic, 0.001f, 0.0f, 1.0f);

                    if (ImGui::Button("Remove"))
                    {
                        m_PointLights.erase(m_PointLights.begin() + i);
                        ImGui::PopID();
                        break;
                    }
                }

                Renderer3D::AddPointLight(m_PointLights[i]);
                ImGui::PopID();
            }

            if (m_PointLights.size() < 32 && ImGui::Button("Add Point Light"))
            {
                PointLight newLight;
                newLight.Position = glm::vec3(0.0f, 2.0f, 2.0f);
                newLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
                newLight.Intensity = 5.0f;
                newLight.Constant = 1.0f;
                newLight.Linear = 0.09f;
                newLight.Quadratic = 0.032f;
                m_PointLights.push_back(newLight);
            }

            ImGui::End();

            // Render Statistics
            ImGui::Begin("3D Render Statistics");
            Renderer3D::Statistics stats = Renderer3D::GetStats();
            ImGui::Text("Draw Calls: %d", stats.DrawCalls);
            ImGui::Text("Model Count: %d", stats.ModelCount);
            ImGui::Text("Mesh Count: %d", stats.MeshCount);
            ImGui::Text("Triangle Count: %d", stats.TriangleCount);
            ImGui::Text("Vertex Count: %d", stats.VertexCount);
            ImGui::Text("Textures Used: %d", stats.TexturesUsed);
            ImGui::Text("Data Size (Bytes): %d", stats.DataSize);

            if (m_CubeModel)
            {
                ImGui::Separator();
                ImGui::Text("Model Info:");
                ImGui::Text("Meshes: %zu", m_CubeModel->GetMeshes().size());
                ImGui::Text("Directory: %s", m_CubeModel->GetDirectory().c_str());
            }
            ImGui::End();

            // Model Information
            if (m_CubeModel && !m_CubeModel->GetMeshes().empty())
            {
                ImGui::Begin("Model Information");
                const auto& meshes = m_CubeModel->GetMeshes();

                for (size_t i = 0; i < meshes.size(); ++i)
                {
                    if (ImGui::CollapsingHeader(("Mesh " + std::to_string(i)).c_str()))
                    {
                        const auto& mesh = meshes[i];
                        ImGui::Text("Vertices: %zu", mesh.Vertices.size());
                        ImGui::Text("Indices: %zu", mesh.Indices.size());
                        ImGui::Text("Triangles: %zu", mesh.Indices.size() / 3);

                        ImGui::Separator();
                        ImGui::Text("Material:");
                        ImGui::Text("Albedo: (%.2f, %.2f, %.2f)",
                            mesh.Mat.Albedo.x, mesh.Mat.Albedo.y, mesh.Mat.Albedo.z);
                        ImGui::Text("Metallic: %.2f", mesh.Mat.Metallic);
                        ImGui::Text("Roughness: %.2f", mesh.Mat.Roughness);
                        ImGui::Text("AO: %.2f", mesh.Mat.AO);

                        ImGui::Text("Textures:");
                        ImGui::Text("  Albedo: %s", mesh.Mat.AlbedoTexture ? "Yes" : "No");
                        ImGui::Text("  Normal: %s", mesh.Mat.NormalTexture ? "Yes" : "No");
                        ImGui::Text("  Metallic: %s", mesh.Mat.MetallicTexture ? "Yes" : "No");
                        ImGui::Text("  Roughness: %s", mesh.Mat.RoughnessTexture ? "Yes" : "No");
                        ImGui::Text("  AO: %s", mesh.Mat.AOTexture ? "Yes" : "No");
                    }
                }
                ImGui::End();
            }
        }

    private:
        // 3D Model and attributes
        Ref<Model> m_CubeModel;
        ModelAttributes m_ModelAttribs;

        // Cameras
        PerspectiveCamera m_PerspectiveCamera;
        OrthographicCamera m_OrthographicCamera;
        SceneCamera m_SceneCamera = SceneCamera::Perspective;

        // Lighting
        DirectionalLight m_DirectionalLight;
        std::vector<PointLight> m_PointLights;

        // Render settings
        RenderMode m_Mode = RenderMode::Normal;

        // Animation
        bool m_AutoRotate = true;
        float m_RotationSpeed = 1.0f; 

        // Utility
        Timer m_FrameTimer;
        float m_FPS = 0; 
    };
}