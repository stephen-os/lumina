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
    class BasicRendering : public Layer
    {
    public:
        virtual void OnAttach() override
        {
			m_PerspectiveCamera = CreateRef<PerspectiveCamera>(45.0f, 1.0f, 0.1f, 100.0f);
            m_PerspectiveCamera->SetPosition(glm::vec3(0.0f, 0.0f, 25.0f));
            m_PerspectiveCamera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

            m_OrthographicCamera = CreateRef<OrthographicCamera>(-5.0f, 5.0f, -5.0f, 5.0f);
            m_OrthographicCamera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
            m_OrthographicCamera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
            m_OrthographicCamera->SetZoom(25.0f);

            m_PerspectiveRenderTarget = Renderer2D::CreateRenderTarget(900, 900);
            m_OrthographicRenderTarget = Renderer2D::CreateRenderTarget(900, 900);

            DockWindowSplit("Perspective Camera", DockPosition::Left, 0.40f);
            DockWindowSubSplit("Perspective Controls", "Perspective Camera", DockPosition::Bottom, 0.30f);

            DockWindowSplit("Orthographic Camera", DockPosition::Center);
            DockWindowSubSplit("Orthographic Controls", "Orthographic Camera", DockPosition::Bottom, 0.30f);

            DockWindowSplit("Performance", DockPosition::Right, 0.20f);
            DockWindowTabbed("Camera Debug", "Performance");
        }

        virtual void OnDetach() override
        {
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

            if (size.x <= 0.0f || size.y <= 0.0f)
            {
                ImGui::End();
                return;
			}
            
            m_PerspectiveRenderTarget->Resize(size.x, size.y);

            m_PerspectiveCamera->SetAspectRatio(size.x / size.y);

            Renderer2D::SetRenderTarget(m_PerspectiveRenderTarget);
            Renderer2D::SetResolution(size.x, size.y);

            Renderer2D::Begin(m_PerspectiveCamera);

            Renderer2D::SetQuadPosition({ 0.0f, 0.0f, 0.0f });
            Renderer2D::SetQuadSize({ 2.0f, 2.0f });
            Renderer2D::SetQuadTintColor({ 1.0f, 0.5f, 0.3f, 1.0f });
            Renderer2D::DrawQuad();

            Renderer2D::SetCirclePosition({ 2.0f, 2.0f, 0.0f });
            Renderer2D::SetCircleRadius({ 1.0f, 1.0f });
            Renderer2D::SetCircleColor({ 0.3f, 0.7f, 1.0f, 0.8f });
            Renderer2D::DrawCircle();

            Renderer2D::SetTrianglePoint1({ -2.0f, -2.0f, 0.0f });
            Renderer2D::SetTrianglePoint2({ -1.0f, -2.0f, 0.0f });
            Renderer2D::SetTrianglePoint3({ -1.5f, -1.0f, 0.0f });
            Renderer2D::SetTriangleColor({ 0.7f, 0.3f, 1.0f, 1.0f });
            Renderer2D::DrawTriangle();

            Renderer2D::SetStringContent("PERSPECTIVE");
            Renderer2D::SetStringPosition({ -3.0f, 4.0f, 0.0f });
            Renderer2D::SetStringColor({ 1.0f, 1.0f, 0.3f, 1.0f });
            Renderer2D::SetStringSize(0.8f);
            Renderer2D::SetStringAlignment(StringAlignment::Left);
            Renderer2D::DrawString();

            Renderer2D::End();

            ImGui::Image(Renderer2D::GetImage(), size);
            Renderer2D::SetRenderTarget(nullptr);

            ImGui::End();
        }

        void RenderOrthographicView()
        {
            ImGui::Begin("Orthographic Camera");
            ImVec2 size = ImGui::GetContentRegionAvail();

            if (size.x <= 0.0f || size.y <= 0.0f)
            {
                ImGui::End();
                return;
            }

            m_OrthographicRenderTarget->Resize(size.x, size.y);

            m_OrthographicCamera->SetSize(size.x, size.y);

            Renderer2D::SetRenderTarget(m_OrthographicRenderTarget);
            Renderer2D::SetResolution(size.x, size.y);
            Renderer2D::Begin(m_OrthographicCamera);

            Renderer2D::SetQuadPosition({ 0.0f, 0.0f, 0.0f });
            Renderer2D::SetQuadSize({ 2.0f, 2.0f });
            Renderer2D::SetQuadTintColor({ 1.0f, 0.5f, 0.3f, 1.0f });
            Renderer2D::DrawQuad();

            Renderer2D::SetCirclePosition({ 2.0f, 2.0f, 0.0f });
            Renderer2D::SetCircleRadius({ 1.0f, 1.0f });
            Renderer2D::SetCircleColor({ 0.3f, 0.7f, 1.0f, 0.8f });
            Renderer2D::DrawCircle();

            Renderer2D::SetTrianglePoint1({ -2.0f, -2.0f, 0.0f });
            Renderer2D::SetTrianglePoint2({ -1.0f, -2.0f, 0.0f });
            Renderer2D::SetTrianglePoint3({ -1.5f, -1.0f, 0.0f });
            Renderer2D::SetTriangleColor({ 0.7f, 0.3f, 1.0f, 1.0f });
            Renderer2D::DrawTriangle();

            Renderer2D::SetStringContent("ORTHOGRAPHIC");
            Renderer2D::SetStringPosition({ -3.0f, 4.0f, 0.0f });
            Renderer2D::SetStringColor({ 1.0f, 1.0f, 0.3f, 1.0f });
            Renderer2D::SetStringSize(0.8f);
            Renderer2D::SetStringAlignment(StringAlignment::Left);
            Renderer2D::DrawString();

            Renderer2D::End();

            ImGui::Image(Renderer2D::GetImage(), size);
            Renderer2D::SetRenderTarget(nullptr);
        
            ImGui::End();
        }

        void RenderControls()
        {
            {
                ImGui::Begin("Perspective Controls");
                glm::vec3 position = m_PerspectiveCamera->GetPosition();
                glm::vec3 rotation = m_PerspectiveCamera->GetRotation();

                if (ImGui::DragFloat3("Camera Position", glm::value_ptr(position), 0.1f))
                {
					m_PerspectiveCamera->SetPosition(position);
                }

                if (ImGui::DragFloat3("Camera Rotation", glm::value_ptr(rotation), 0.1f))
                {
					m_PerspectiveCamera->SetRotation(rotation);
                }

                if (ImGui::Button("Reset Perspective Camera"))
                {
                    m_PerspectiveCamera->SetPosition(glm::vec3(0.0f, 0.0f, 25.0f));
                    m_PerspectiveCamera->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
                    m_PerspectiveCamera->LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
                }

                ImGui::End();
            }

            {
                ImGui::Begin("Orthographic Controls");
                glm::vec3 position = m_OrthographicCamera->GetPosition();
                glm::vec3 rotation = m_OrthographicCamera->GetRotation();
                float zoom = m_OrthographicCamera->GetZoom();

                if (ImGui::DragFloat3("Camera Position", glm::value_ptr(position), 0.1f))
                {
					m_OrthographicCamera->SetPosition(position);
                }

                if (ImGui::DragFloat3("Camera Rotation", glm::value_ptr(rotation), 0.1f))
                {
					m_OrthographicCamera->SetRotation(rotation);
                }

                if (ImGui::DragFloat("Zoom", &zoom, 0.1f, 0.1f, 50.0f))
                {
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
			Renderer2D::ResetStats();

            ImGui::End();
            
            ImGui::Begin("Camera Debug");

            if (ImGui::CollapsingHeader("Perspective Camera"))
            {
                glm::vec3 pos = m_PerspectiveCamera->GetPosition();
                glm::vec3 rot = m_PerspectiveCamera->GetRotation();
                ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
                ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", rot.x, rot.y, rot.z);

                glm::vec3 forward = m_PerspectiveCamera->GetForward();
                glm::vec3 right = m_PerspectiveCamera->GetRight();
                glm::vec3 up = m_PerspectiveCamera->GetUp();
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
                    m_OrthographicCamera->GetTop()
                );
            }

            ImGui::End();   
        }

    private:
        Ref<PerspectiveCamera> m_PerspectiveCamera;
        Ref<OrthographicCamera> m_OrthographicCamera;

        Ref<RenderTarget> m_PerspectiveRenderTarget;
        Ref<RenderTarget> m_OrthographicRenderTarget;

        Timer m_FrameTimer;
        float m_FPS = 0.0f;
    };
}