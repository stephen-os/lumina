#include "Application.h"

#include <iostream>
#include <vector> 

#include <spdlog/spdlog.h>

#include <stb/stb_image.h>

#include <fstream>
#include <filesystem>
#include <stdlib.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Log.h"
#include "Assert.h"
#include "Theme.h"

#include "../Graphics/Renderer2D.h"
#include "../Graphics/Renderer3D.h"

#include <glad/glad.h>

namespace Lumina
{
    static Lumina::Application* s_Instance = nullptr;

    Application& Application::GetInstance() { return *s_Instance; }

    Application::Application(const ApplicationSpecification& applicationSpecification)
    {
        s_Instance = this;
        m_Specifications = applicationSpecification;

        Log::Init(m_Specifications.Name);
        LUMINA_LOG_INFO("Starting Lumina Application: {}", m_Specifications.Name);

        // Create window
        WindowSpecification windowSpec;
        windowSpec.Title = m_Specifications.Name;
        windowSpec.IconPath = m_Specifications.Icon;
        windowSpec.Width = m_Specifications.Width;
        windowSpec.Height = m_Specifications.Height;
        windowSpec.PositionX = m_Specifications.PositionX;
        windowSpec.PositionY = m_Specifications.PositionY;
        windowSpec.Fullscreen = m_Specifications.Fullscreen;
        windowSpec.Maximized = m_Specifications.Maximized;

        m_Window = std::make_unique<Window>(windowSpec);

        if (m_Specifications.Use2DRenderer)
            Renderer2D::Init();

        if (m_Specifications.Use3DRenderer)
            Renderer3D::Init();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplGlfw_InitForOpenGL(m_Window->GetNativeWindow(), true);
        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Apply Theme
        if (m_Specifications.Theme)
            Theme::ApplyLuminaTheme();
    }


    Application::~Application()
    {
        s_Instance = nullptr;

        for (auto& layer : m_LayerStack)
            layer->OnDetach();

        if (m_Specifications.Use2DRenderer)
            Renderer2D::Shutdown();

        if (m_Specifications.Use3DRenderer)
            Renderer3D::Shutdown();

        m_LayerStack.clear();

        ImGui_ImplOpenGL3_Shutdown();

        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Application::Run()
    {
        float lastTime = GetTime();

        while (m_Running)
        {
            m_Window->Update();

            if (m_Window->ShouldClose())
            {
                Shutdown();
                break;
            }

            float currentTime = GetTime();
            float timestep = glm::clamp(currentTime - lastTime, 0.001f, 0.1f);
            lastTime = currentTime;

            for (auto& layer : m_LayerStack)
                layer->OnUpdate(timestep);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (m_Specifications.EnableDocking)
            {
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);

                ImGuiWindowFlags dockspace_flags =
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoBringToFrontOnFocus |
                    ImGuiWindowFlags_NoNavFocus |
                    ImGuiWindowFlags_NoBackground;

                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

                ImGui::Begin("DockSpace", nullptr, dockspace_flags);

                m_DockspaceID = ImGui::GetID("MainDockSpace");
                ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

                ImGui::End();
                ImGui::PopStyleVar(3);
            }

            for (auto& layer : m_LayerStack)
                layer->OnUIRender();

            ImGui::Render();

            ImDrawData* main_draw_data = ImGui::GetDrawData();
            const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);

            if (!main_is_minimized)
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }

            // Handle ImGui viewport if enabled
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            if (!main_is_minimized)
                m_Window->SwapBuffers();
        }
    }

    void Application::SetWindowFullscreen()
    {
        m_Specifications.Fullscreen = !m_Specifications.Fullscreen;
        m_Window->SetFullscreen(m_Specifications.Fullscreen);
    }

    float Application::GetTime()
    {
        return (float)glfwGetTime();
	}
}