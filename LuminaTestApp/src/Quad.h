#include <vector>
#include <iostream>
#include <string>
#include <memory>

#include "imgui.h"

#include "Lumina/Core/Layer.h"
#include "Lumina/Core/Ref.h"
#include "Lumina/Core/Log.h"
#include "Lumina/Utils/Timer.h"

#include "Lumina/Graphics/Buffer.h"
#include "Lumina/Graphics/VertexArray.h"
#include "Lumina/Graphics/ShaderProgram.h"
#include "Lumina/Graphics/FrameBuffer.h"
#include "Lumina/Graphics/RenderCommands.h"

#include "Lumina/Graphics/Cameras/OrthographicCamera.h"
#include "Lumina/Graphics/Cameras/PerspectiveCamera.h"

#include "Lumina/Utils/FileReader.h"
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Dev Dependencies
#include "Lumina/Core/Input.h"
#include "Lumina/Core/KeyCode.h"

class Quad : public Lumina::Layer
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

        auto vbo = Lumina::VertexBuffer::Create(vertices, sizeof(vertices));
		auto ibo = Lumina::IndexBuffer::Create(indices, sizeof(indices));

        auto layout = Lumina::BufferLayout({
			{ Lumina::BufferDataType::Float2, "a_Position" }
		});

		vbo->SetLayout(layout);

		m_VertexArray = Lumina::VertexArray::Create();
		m_VertexArray->SetVertexBuffer(vbo);
		m_VertexArray->SetIndexBuffer(ibo);

        m_FrameBuffer = Lumina::FrameBuffer::Create();
        m_FrameBuffer->Resize(800, 800);

        std::string vertexShader = Lumina::ReadFile("res/shaders/Simple.vert");
		std::string fragmentShader = Lumina::ReadFile("res/shaders/Simple.frag");

        m_Shader = Lumina::ShaderProgram::Create(vertexShader, fragmentShader);

        // Create camera 
        float width = 800.0f;
        float height = 800.0f;
        float aspect = width / height;
        float orthoHeight = 1.0f;
        float orthoWidth = orthoHeight * aspect;

        m_Camera.SetOrthographicProjection(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -1.0f, 1.0f);
    }

    virtual void OnDetach() override {}

    virtual void OnUpdate(float ts) override
    {

    }

    virtual void OnUIRender() override
    {
        ImGui::Begin("Test");
        ImVec2 size = ImGui::GetContentRegionAvail();
        
        m_FrameBuffer->Bind();

        m_Shader->Bind();
        glm::mat4 viewProjection = m_Camera.GetViewMatrix() * m_Camera.GetProjectionMatrix(); 
        m_Shader->SetUniformMat4("u_ViewProjection", viewProjection);

        Lumina::RenderCommands::DrawTriangles(m_VertexArray); 
        
        m_Shader->Unbind();

        ImGui::Image((void*)m_FrameBuffer->GetColorAttachment(), size);
		m_FrameBuffer->Unbind();

        ImGui::End(); 
    }
private:
    Lumina::Ref<Lumina::VertexArray> m_VertexArray;
    Lumina::Ref<Lumina::ShaderProgram> m_Shader;
	Lumina::Ref<Lumina::FrameBuffer> m_FrameBuffer;

    Lumina::OrthographicCamera m_Camera;
    
    Lumina::Timer m_FrameTimer;
    float m_FPS = 0.0f;
};
