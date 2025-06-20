#include <vector>
#include <iostream>
#include <string>
#include <memory>

#include "imgui.h"

#include "Lumina/Core/Layer.h"
#include "Lumina/Core/Log.h"
#include "Lumina/Utils/Timer.h"
#include "Lumina/Graphics/Renderer.h"
#include "Lumina/Graphics/TextureAtlas.h"
#include "Lumina/Core/Application.h"

#include "Lumina/Graphics/Cameras/OrthographicCamera.h"
#include "Lumina/Graphics/Cameras/PerspectiveCamera.h"

#include "Lumina/Utils/FileReader.h"
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Dev Dependencies
#include "Lumina/Core/Input.h"
#include "Lumina/Core/KeyCode.h"

class Rendering : public Lumina::Layer
{
public:
    virtual void OnAttach() override
    {
        Lumina::Renderer::Init();

        std::string source = "res/texture/factory_atlas.png";
        m_Atlas = Lumina::TextureAtlas::Create(source, 16, 16);

        m_Quad.Texture = m_Atlas->GetTexture();
        m_Quad.Position = { 0.8f, 0.0f, 0.0f };
        m_Quad.Size = { 1.0f, 1.0f };
        m_Quad.TextureCoords = m_Atlas->GetTextureCoords(0);

        m_Cube.Texture = m_Atlas->GetTexture();
        m_Cube.TextureCoords = m_Atlas->GetTextureCoords(1);
    }

    virtual void OnDetach() override {}

    virtual void OnUpdate(float ts) override
    {
        float elapsedTime = m_FrameTimer.Elapsed();
        m_FPS = 1.0f / elapsedTime;
        m_FrameTimer.Reset();

        Lumina::Input::SetCursorMode(Lumina::CursorMode::Normal);

        /*
        // Keyboard movement
        float velocity = m_CameraSpeed;
        if (Lumina::Input::IsKeyPressed(Lumina::Key::W))
            m_CameraPosition += m_CameraFront * velocity;
        if (Lumina::Input::IsKeyPressed(Lumina::Key::S))
            m_CameraPosition -= m_CameraFront * velocity;
        if (Lumina::Input::IsKeyPressed(Lumina::Key::A))
            m_CameraPosition -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * velocity;
        if (Lumina::Input::IsKeyPressed(Lumina::Key::D))
            m_CameraPosition += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * velocity;
        if (Lumina::Input::IsKeyPressed(Lumina::Key::Escape))
            Lumina::Application::GetInstance().Shutdown();
        if (Lumina::Input::IsKeyPressed(Lumina::Key::M))
            Lumina::Input::SetCursorMode(Lumina::CursorMode::Disabled);
        if (Lumina::Input::IsKeyPressed(Lumina::Key::N))
            Lumina::Input::SetCursorMode(Lumina::CursorMode::Normal);
        

        // Mouse movement
        auto [xpos, ypos] = Lumina::Input::GetMousePosition();

        if (m_FirstMouse)
        {
            m_LastX = xpos;
            m_LastY = ypos;
            m_FirstMouse = false;
        }

        float xoffset = xpos - m_LastX;
        float yoffset = m_LastY - ypos;
        m_LastX = xpos;
        m_LastY = ypos;

        xoffset *= m_Sensitivity;
        yoffset *= m_Sensitivity;

        m_Yaw += xoffset;
        m_Pitch += -yoffset;

        // Clamp pitch
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
        */

        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_CameraFront = glm::normalize(front);

        // Final camera matrix
        glm::mat4 view = glm::lookAt(m_CameraPosition, m_CameraPosition + m_CameraFront, m_CameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), m_ViewportSize.x / m_ViewportSize.y, 0.1f, 100.0f);
        m_ViewProjectionMatrix = projection * view;
    }

    virtual void OnUIRender() override
    {
        ImGui::Begin("Example Window");
        ImGui::SetCursorPos({ 0.0f, 0.0f });

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        m_ViewportSize = { viewportSize.x, viewportSize.y };

        Lumina::Renderer::SetResolution(viewportSize.x, viewportSize.y);

        Lumina::Renderer::Begin(m_ViewProjectionMatrix);

        Lumina::Renderer::DrawQuad(m_Quad);
        Lumina::Renderer::DrawCube(m_Cube);

        Lumina::Renderer::End();

        ImGui::Image((void*)(intptr_t)Lumina::Renderer::GetImage(), { viewportSize.x, viewportSize.y });
        ImGui::End();

        ImGui::Begin("Statistic");
        Lumina::Renderer::Statistics stats = Lumina::Renderer::GetStats();
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quad Count: %d", stats.QuadCount);
        ImGui::Text("Shaders Used: %d", stats.ShadersUsed);
        ImGui::Text("Textures Used: %d", stats.TexturesUsed);
        ImGui::Text("Data Size (Bytes): %d", stats.DataSize);
        Lumina::Renderer::ResetStats();
        ImGui::End();

        ImGui::Begin("Attribute Controls");
        ImGui::Text("Quad");
        ImGui::DragFloat3("Q Position", &m_Quad.Position.x, 0.1f);
        ImGui::DragFloat3("Q Rotation", &m_Quad.Rotation.x, 0.1f);
        ImGui::DragFloat2("Q Size", &m_Quad.Size.x, 0.1f);
        ImGui::Text("Cube");
        ImGui::DragFloat3("C Position", &m_Cube.Position.x, 0.1f);
        ImGui::DragFloat3("C Rotation", &m_Cube.Rotation.x, 0.1f);
        ImGui::DragFloat3("C Size", &m_Cube.Size.x, 0.1f);
        ImGui::ColorPicker4("Tint", &m_Cube.TintColor.x);
        ImGui::End();

        ImGui::Begin("Camera Stats");
        ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", m_CameraPosition.x, m_CameraPosition.y, m_CameraPosition.z);
        ImGui::Text("Camera Front: (%.1f, %.1f, %.1f)", m_CameraFront.x, m_CameraFront.y, m_CameraFront.z);
        ImGui::Text("Camera Up: (%.1f, %.1f, %.1f)", m_CameraUp.x, m_CameraUp.y, m_CameraUp.z);
        ImGui::Text("Camera Speed: %.1f", m_CameraSpeed);
        ImGui::End();

        ImGui::Begin("Texture Atlas");
        ImVec2 size = { 200, 200 }; 
        ImGui::Image((void*)m_Atlas->GetTexture()->GetID(), size);
        ImGui::End(); 
    }
private:
    Lumina::QuadAttributes m_Quad;
    Lumina::CubeAttributes m_Cube;

    glm::vec3 m_CameraPosition = glm::vec3(0.0f, 0.0f, 10.0f);
    glm::vec3 m_CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float m_CameraSpeed = 0.05f;

    bool m_FirstMouse = true;
    float m_LastX = 400.0f, m_LastY = 300.0f; // center of the screen
    float m_Yaw = -90.0f; // initialized to look toward -Z
    float m_Pitch = 0.0f;
    float m_Sensitivity = 0.05f;

    glm::vec2 m_ViewportSize = { 3.0f, 4.0f };

    glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

    glm::vec4 m_TintColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    Lumina::Ref<Lumina::TextureAtlas> m_Atlas;
    Lumina::Timer m_FrameTimer;
    float m_FPS = 0.0f;
};