#include <vector>
#include <iostream>
#include <string>
#include <memory>

#include "imgui.h"

#include "Lumina/Lumina.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Lumina
{
    class DemoBasicRendering : public Layer
    {
    public:
        virtual void OnAttach() override
        {
            // Initialize Renderer2D
            Renderer2D::Init();

            // Setup cameras
            m_PerspectiveCamera.SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
            m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

            m_OrthographicCamera = CreateRef<OrthographicCamera>(-5.0f, 5.0f, -5.0f, 5.0f);
            m_OrthographicCamera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
            m_OrthographicCamera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            m_OrthographicCamera->SetZoom(25.0f);

            // Create render targets for each camera view
            m_PerspectiveRenderTarget = Renderer2D::CreateRenderTarget(900, 900);
            m_OrthographicRenderTarget = Renderer2D::CreateRenderTarget(900, 900);
        }

        virtual void OnDetach() override
        {
            Renderer2D::Shutdown();
        }

        virtual void OnUpdate(float ts) override
        {
            m_FPS = 1.0f / m_FrameTimer.Elapsed();
            m_FrameTimer.Reset();
        }

        virtual void OnUIRender() override
        {
            RenderPerspectiveView();
            RenderOrthographicView();
            RenderControls();
            RenderDebugInfo();
        }

    private:
        void RenderPerspectiveView()
        {
            ImGui::Begin("Perspective Camera");
            ImVec2 size = ImGui::GetContentRegionAvail();

            if (size.x > 0 && size.y > 0)
            {
                // Resize render target if needed
                m_PerspectiveRenderTarget->Resize(size.x, size.y);

                // Update camera aspect ratio
                m_PerspectiveCamera.SetAspectRatio(size.x / size.y);

                // Set render target and begin rendering
                Renderer2D::SetRenderTarget(m_PerspectiveRenderTarget);
                Renderer2D::SetResolution(size.x, size.y);

                // Convert PerspectiveCamera to use with Renderer2D
                glm::mat4 viewProjection = m_PerspectiveCamera.GetProjectionMatrix() * m_PerspectiveCamera.GetViewMatrix();
                Renderer2D::Begin(viewProjection);

                // Draw a simple quad
                Renderer2D::SetQuadPosition({ 0.0f, 0.0f, 0.0f });
                Renderer2D::SetQuadSize({ 2.0f, 2.0f });
                Renderer2D::SetQuadTintColor({ 1.0f, 0.5f, 0.3f, 1.0f }); // Orange color
                Renderer2D::DrawQuad();

                // Draw some additional shapes for visual interest
                Renderer2D::SetCirclePosition({ 2.0f, 2.0f, 0.0f });
                Renderer2D::SetCircleRadius({ 1.0f, 1.0f });
                Renderer2D::SetCircleColor({ 0.3f, 0.7f, 1.0f, 0.8f }); // Blue circle
                Renderer2D::DrawCircle();

                Renderer2D::SetTrianglePoint1({ -2.0f, -2.0f, 0.0f });
                Renderer2D::SetTrianglePoint2({ -1.0f, -2.0f, 0.0f });
                Renderer2D::SetTrianglePoint3({ -1.5f, -1.0f, 0.0f });
                Renderer2D::SetTriangleColor({ 0.7f, 0.3f, 1.0f, 1.0f }); // Purple triangle
                Renderer2D::DrawTriangle();

                Renderer2D::End();
                Renderer2D::SetRenderTarget(nullptr);

                // Display the rendered image
                ImGui::Image((void*)(intptr_t)m_PerspectiveRenderTarget->GetTexture(), size);
            }

            ImGui::End();
        }

        void RenderOrthographicView()
        {
            ImGui::Begin("Orthographic Camera");
            ImVec2 size = ImGui::GetContentRegionAvail();

            if (size.x > 0 && size.y > 0)
            {
                // Resize render target if needed
                m_OrthographicRenderTarget->Resize(size.x, size.y);

                // Update camera size
                m_OrthographicCamera->SetSize(size.x, size.y);

                // Set render target and begin rendering
                Renderer2D::SetRenderTarget(m_OrthographicRenderTarget);
                Renderer2D::SetResolution(size.x, size.y);
                Renderer2D::Begin(m_OrthographicCamera);

                // Draw the same shapes but they'll look different due to orthographic projection
                Renderer2D::SetQuadPosition({ 0.0f, 0.0f, 0.0f });
                Renderer2D::SetQuadSize({ 2.0f, 2.0f });
                Renderer2D::SetQuadTintColor({ 1.0f, 0.5f, 0.3f, 1.0f }); // Orange color
                Renderer2D::DrawQuad();

                Renderer2D::SetCirclePosition({ 2.0f, 2.0f, 0.0f });
                Renderer2D::SetCircleRadius({ 1.0f, 1.0f });
                Renderer2D::SetCircleColor({ 0.3f, 0.7f, 1.0f, 0.8f }); // Blue circle
                Renderer2D::DrawCircle();

                Renderer2D::SetTrianglePoint1({ -2.0f, -2.0f, 0.0f });
                Renderer2D::SetTrianglePoint2({ -1.0f, -2.0f, 0.0f });
                Renderer2D::SetTrianglePoint3({ -1.5f, -1.0f, 0.0f });
                Renderer2D::SetTriangleColor({ 0.7f, 0.3f, 1.0f, 1.0f }); // Purple triangle
                Renderer2D::DrawTriangle();

                // Add some text
                Renderer2D::SetStringContent("ORTHOGRAPHIC");
                Renderer2D::SetStringPosition({ -3.0f, 4.0f, 0.0f });
                Renderer2D::SetStringColor({ 1.0f, 1.0f, 0.3f, 1.0f });
                Renderer2D::SetStringSize(0.8f);
                Renderer2D::SetStringAlignment(StringAlignment::Left);
                Renderer2D::DrawString();

                Renderer2D::End();
                Renderer2D::SetRenderTarget(nullptr);

                // Display the rendered image
                ImGui::Image((void*)(intptr_t)m_OrthographicRenderTarget->GetTexture(), size);
            }

            ImGui::End();
        }

        void RenderControls()
        {
            // Perspective Camera Controls
            {
                ImGui::Begin("Perspective Controls");
                glm::vec3 position = m_PerspectiveCamera.GetPosition();
                glm::vec3 rotation = m_PerspectiveCamera.GetRotation();

                bool updated = false;
                updated |= ImGui::DragFloat3("Camera Position", glm::value_ptr(position), 0.1f);
                updated |= ImGui::DragFloat3("Camera Rotation", glm::value_ptr(rotation), 0.1f);

                if (updated)
                {
                    m_PerspectiveCamera.SetPosition(position);
                    m_PerspectiveCamera.SetRotation(rotation);
                }

                if (ImGui::Button("Reset Perspective Camera"))
                {
                    m_PerspectiveCamera.SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
                    m_PerspectiveCamera.SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
                    m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }

                ImGui::End();
            }

            // Orthographic Camera Controls
            {
                ImGui::Begin("Orthographic Controls");
                glm::vec3 position = m_OrthographicCamera->GetPosition();
                glm::vec3 rotation = m_OrthographicCamera->GetRotation();
                float zoom = m_OrthographicCamera->GetZoom();

                bool updated = false;
                updated |= ImGui::DragFloat3("Camera Position", glm::value_ptr(position), 0.1f);
                updated |= ImGui::DragFloat3("Camera Rotation", glm::value_ptr(rotation), 0.1f);
                updated |= ImGui::DragFloat("Zoom", &zoom, 0.1f, 0.1f, 50.0f);

                if (updated)
                {
                    m_OrthographicCamera->SetPosition(position);
                    m_OrthographicCamera->SetRotation(rotation);
                    m_OrthographicCamera->SetZoom(zoom);
                }

                if (ImGui::Button("Reset Orthographic Camera"))
                {
                    m_OrthographicCamera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
                    m_OrthographicCamera->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
                    m_OrthographicCamera->SetZoom(25.0f);
                    m_OrthographicCamera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }

                ImGui::End();
            }
        }

        void RenderDebugInfo()
        {
            // FPS Display
            {
                ImGui::Begin("Performance");
                ImGui::Text("FPS: %.2f", m_FPS);

                auto stats = Renderer2D::GetStats();
                ImGui::Separator();
                ImGui::Text("Renderer2D Statistics:");
                ImGui::Text("Draw Calls: %d", stats.DrawCalls);
                ImGui::Text("Quads: %d", stats.QuadCount);
                ImGui::Text("Circles: %d", stats.CircleCount);
                ImGui::Text("Triangles: %d", stats.TriangleCount);
                ImGui::Text("Text: %d", stats.TextCount);
                ImGui::Text("Total Vertices: %d", stats.GetTotalVertexCount());
                ImGui::Text("Data Size: %d bytes", stats.DataSize);

                ImGui::End();
            }

            // Camera Debug Info
            {
                ImGui::Begin("Camera Debug");

                if (ImGui::CollapsingHeader("Perspective Camera"))
                {
                    glm::vec3 pos = m_PerspectiveCamera.GetPosition();
                    glm::vec3 rot = m_PerspectiveCamera.GetRotation();
                    ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
                    ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", rot.x, rot.y, rot.z);

                    glm::vec3 forward = m_PerspectiveCamera.GetForward();
                    glm::vec3 right = m_PerspectiveCamera.GetRight();
                    glm::vec3 up = m_PerspectiveCamera.GetUp();
                    ImGui::Text("Forward: (%.2f, %.2f, %.2f)", forward.x, forward.y, forward.z);
                    ImGui::Text("Right: (%.2f, %.2f, %.2f)", right.x, right.y, right.z);
                    ImGui::Text("Up: (%.2f, %.2f, %.2f)", up.x, up.y, up.z);
                }

                if (ImGui::CollapsingHeader("Orthographic Camera"))
                {
                    glm::vec3 pos = m_OrthographicCamera->GetPosition();
                    glm::vec3 rot = m_OrthographicCamera->GetRotation();
                    float zoom = m_OrthographicCamera->GetZoom();
                    ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
                    ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", rot.x, rot.y, rot.z);
                    ImGui::Text("Zoom: %.2f", zoom);

                    ImGui::Text("Bounds: L:%.1f R:%.1f B:%.1f T:%.1f",
                        m_OrthographicCamera->GetLeft(),
                        m_OrthographicCamera->GetRight(),
                        m_OrthographicCamera->GetBottom(),
                        m_OrthographicCamera->GetTop());
                }

                ImGui::End();
            }
        }

    private:
        // Cameras
        PerspectiveCamera m_PerspectiveCamera;
        Ref<OrthographicCamera> m_OrthographicCamera;

        // Render targets
        Ref<RenderTarget> m_PerspectiveRenderTarget;
        Ref<RenderTarget> m_OrthographicRenderTarget;

        // Performance tracking
        Timer m_FrameTimer;
        float m_FPS = 0.0f;
    };
}