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

#include <Lumina/Events/ApplicationEvent.h>
#include <Lumina/Events/MouseEvent.h>
#include <Lumina/Events/KeyEvent.h>

#include <glad/glad.h>
#include <glm/glm.hpp>

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

        // Set event callback for window
        m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplGlfw_InitForOpenGL(m_Window->GetNativeWindow(), true);
        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);

        if (m_Specifications.Theme)
            Theme::ApplyLuminaTheme();
    }

    Application::~Application()
    {
        s_Instance = nullptr;

        for (auto& layer : m_LayerStack)
            layer->OnDetach();

        m_LayerStack.clear();

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Application::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);

        // Handle application-level events
        dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) {
            m_Running = false;
            return true;
            });

        dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) {
            LUMINA_LOG_INFO("Window resized: {}x{}", e.GetWidth(), e.GetHeight());
            return false;
            });

        // Propagate to layers in reverse order (top to bottom)
        // This allows UI layers to handle events before game layers
        for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
        {
            if (e.Handled)
                break;
            (*it)->OnEvent(e);
        }
    }

    void Application::PostEvent(Event& e)
    {
        OnEvent(e);
    }

    void Application::QueueEvent(std::unique_ptr<Event> e)
    {
        m_EventQueue.push_back(std::move(e));
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

            // Process queued events
            for (auto& event : m_EventQueue)
                OnEvent(*event);
            m_EventQueue.clear();

            float currentTime = GetTime();
            float timestep = glm::clamp(currentTime - lastTime, 0.001f, 0.1f);
            lastTime = currentTime;

            for (auto& layer : m_LayerStack)
                layer->OnUpdate(timestep);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);

            ImGuiWindowFlags window_flags =
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

            ImGui::Begin("LuminaWindow", nullptr, window_flags);

            ImGui::PopStyleVar(3);

            if (m_Specifications.EnableDocking)
            {
                m_DockspaceID = ImGui::GetID("LuminaDockspace");
                ImGui::DockSpace(m_DockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

                for (auto& layer : m_LayerStack)
                {
                    if (layer->HasDockingRequests())
                        layer->ProcessDockingRequests(m_DockspaceID);
                }
            }

            for (auto& layer : m_LayerStack)
                layer->OnUIRender();

            ImGui::End();

            ImGui::Render();

            ImDrawData* main_draw_data = ImGui::GetDrawData();
            const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);

            if (!main_is_minimized)
            {
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }

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