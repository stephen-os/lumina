#include <vector>
#include <iostream>
#include <string>
#include <memory>

#include "imgui.h"

#include "Lumina/Core/Layer.h"
#include "Lumina/Core/Ref.h"
#include "Lumina/Utils/Timer.h"

#include "Lumina/Graphics/Buffer.h"
#include "Lumina/Graphics/VertexArray.h"
#include "Lumina/Graphics/ShaderProgram.h"
#include "Lumina/Graphics/FrameBuffer.h"
#include "Lumina/Graphics/RenderCommands.h"

#include "Lumina/Graphics/Cameras/OrthographicCamera.h"
#include "Lumina/Graphics/Cameras/PerspectiveCamera.h"
#include "Lumina/Graphics/Cameras/Camera.h"

#include "Lumina/Utils/FileReader.h"

#include <glm/glm.hpp>

namespace Lumina
{
    class BasicRendering : public Layer
    {
    public:
        virtual void OnAttach() override
        {
            float vertices[] = 
            {
                -0.5f, -0.5f,
                 0.5f, -0.5f,
                 0.5f,  0.5f,
                -0.5f,  0.5f
            };

            uint32_t indices[] = 
            {
                0, 1, 2,
                2, 3, 0
            };

            auto vbo = VertexBuffer::Create(vertices, sizeof(vertices));
		    auto ibo = IndexBuffer::Create(indices, sizeof(indices));

            auto layout = BufferLayout({
			    { BufferDataType::Float2, "a_Position" }
		    });

		    vbo->SetLayout(layout);

		    m_VertexArray = VertexArray::Create();
		    m_VertexArray->SetVertexBuffer(vbo);
		    m_VertexArray->SetIndexBuffer(ibo);

		    m_PerspectiveFrameBuffer = FrameBuffer::Create();
		    m_PerspectiveFrameBuffer->Resize(900, 900);

		    m_OrthographicFrameBuffer = FrameBuffer::Create();
		    m_OrthographicFrameBuffer->Resize(900, 900);

            std::string vertexShader = ReadFile("res/shaders/Basic.vert");
		    std::string fragmentShader = ReadFile("res/shaders/Basic.frag");

            m_Shader = ShaderProgram::Create(vertexShader, fragmentShader);

            m_PerspectiveCamera.SetPosition(glm::vec3(0.0f, 0.0f, 5.0f));
            m_PerspectiveCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));

		    m_OrthographicCamera.SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
		    m_OrthographicCamera.LookAt(glm::vec3(0.0f, 0.0f, 0.0f));
		    m_OrthographicCamera.SetZoom(4.0f);
        }

        virtual void OnDetach() override {}

        virtual void OnUpdate(float ts) override 
        {
            m_FPS = 1.0f / m_FrameTimer.Elapsed();
            m_FrameTimer.Reset();
        }

        virtual void OnUIRender() override
        {
            {
                ImGui::Begin("Perspective Camera");
                ImVec2 size = ImGui::GetContentRegionAvail();

                if (size.x > 0 && size.y > 0)
                {
                    m_PerspectiveFrameBuffer->Bind();
                    m_PerspectiveFrameBuffer->Resize(size.x, size.y);

                    m_PerspectiveCamera.SetAspectRatio(size.x / size.y);

                    m_Shader->Bind();
                    m_Shader->SetUniformMat4("u_ViewProjection", m_PerspectiveCamera.GetProjectionMatrix() * m_PerspectiveCamera.GetViewMatrix());

                    RenderCommands::SetViewport(0, 0, size.x, size.y);
                    RenderCommands::Clear();
                    RenderCommands::DrawTriangles(m_VertexArray);

                    m_Shader->Unbind();

                    ImGui::Image((void*)m_PerspectiveFrameBuffer->GetColorAttachment(), size);
                    m_PerspectiveFrameBuffer->Unbind();
                }
               
                ImGui::End();
            }

            {
                ImGui::Begin("Orthographic Camera");
                ImVec2 size = ImGui::GetContentRegionAvail();

                if (size.x > 0 && size.y > 0)
                {
                    m_OrthographicFrameBuffer->Bind();
                    m_OrthographicFrameBuffer->Resize(size.x, size.y);

                    m_Shader->Bind();
                    m_Shader->SetUniformMat4("u_ViewProjection", m_OrthographicCamera.GetProjectionMatrix() * m_OrthographicCamera.GetViewMatrix());

                    RenderCommands::SetViewport(0, 0, size.x, size.y);
                    RenderCommands::Clear();
                    RenderCommands::DrawTriangles(m_VertexArray);

                    m_Shader->Unbind();

                    ImGui::Image((void*)m_OrthographicFrameBuffer->GetColorAttachment(), size);
                    m_OrthographicFrameBuffer->Unbind();
                }

                ImGui::End();
            }

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

		    {
			    ImGui::Begin("FPS");
			    ImGui::Text("FPS: %.2f", m_FPS);
			    ImGui::End();
		    }

            ImGui::Begin("Perspective Camera Debug");
            ImGui::Text(Camera::MatrixToString(m_PerspectiveCamera.GetProjectionMatrix()).c_str());
            ImGui::Text(Camera::MatrixToString(m_PerspectiveCamera.GetViewMatrix()).c_str());
            ImGui::End();

            ImGui::Begin("Orthographic Camera Debug");
            ImGui::Text(Camera::MatrixToString(m_OrthographicCamera.GetProjectionMatrix()).c_str());
            ImGui::Text(Camera::MatrixToString(m_OrthographicCamera.GetViewMatrix()).c_str());
            ImGui::End();
        }
    private:
        Ref<VertexArray> m_VertexArray;
        Ref<ShaderProgram> m_Shader;

	    Ref<FrameBuffer> m_PerspectiveFrameBuffer;
	    Ref<FrameBuffer> m_OrthographicFrameBuffer;
	
        PerspectiveCamera m_PerspectiveCamera;
	    OrthographicCamera m_OrthographicCamera;
    
        Timer m_FrameTimer;
        float m_FPS = 0.0f;
    };
}