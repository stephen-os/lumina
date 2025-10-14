#include <vector>
#include <iostream>
#include <string>
#include <memory>

#include "imgui.h"

#include <glm/gtc/type_ptr.hpp>

#include "Lumina/Graphics/Renderer2D.h"

#include "Lumina/Graphics/Cameras/OrthographicCamera.h"

namespace Lumina
{
    class TestRenderer2D : public Layer
    {
    public:
        virtual void OnAttach() override
        {
			Renderer2D::Init();

			m_Camera = OrthographicCamera::Create(-4.0f, 4.0f, -4.0f, 4.0f, -100.0f, 100.0f);
			m_Camera->SetPosition(m_CameraPosition);

			DockWindowSplit("Viewport", DockPosition::Center);
			DockWindowSplit("Settings", DockPosition::Right, 0.2f);
        }

        virtual void OnDetach() override
        {
			Renderer2D::Shutdown();
        }

        virtual void OnUpdate(float ts) override
        {
			Renderer2D::Begin(m_Camera);

			Renderer2D::SetQuadPosition({ 0.0f, 0.0f, 0.0f });
			Renderer2D::SetQuadSize({ 2.0f, 2.0f });
			Renderer2D::SetQuadTintColor({ 1.0f, 0.0f, 0.0f, 1.0f });
            Renderer2D::DrawQuad();

			Renderer2D::End();
        }

        virtual void OnUIRender() override
        {
			ImGui::Begin("Viewport");

			auto size = ImGui::GetContentRegionAvail();
            m_Camera->SetViewportSize(size.x, size.y);
            // Renderer2D::SetResolution(size.x, size.y);

			ImGui::Image(Renderer2D::GetImage(), size, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();

			ImGui::Begin("Settings");

			ImGui::Text("Camera Controls");
			if (ImGui::DragFloat3("Position", glm::value_ptr(m_CameraPosition), 0.1f))
			{
				m_Camera->SetPosition(m_CameraPosition);
			}
			if (ImGui::DragFloat("Zoom", &m_CameraZoom, 0.1f, 0.1f, 2.0f))
			{
				// m_Camera->SetZoom(m_CameraZoom);
			}
			ImGui::End();
        }

    private:
		Ref<OrthographicCamera> m_Camera;

		// Camera controls
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 3.0f };
		float m_CameraZoom = 1.0f;
    };
}