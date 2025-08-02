#include "Application.h"

#include <iostream>
#include <vector> 

#include <spdlog/spdlog.h>

#include <fstream>
#include <filesystem>
#include <stdlib.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "Log.h"
#include "Assert.h"

#include "../Graphics/Renderer2D.h"
#include "../Graphics/Renderer3D.h"

#include <glad/glad.h>

namespace Lumina
{
    static Lumina::Application* s_Instance = nullptr;

    Application& Application::GetInstance() { return *s_Instance; }

    static void GLFWErrorCallback(int error, const char* description)
    {
        LUMINA_LOG_ERROR("[GLFW ERROR] {}: {}", error, description);
    }

    Application::Application(const ApplicationSpecification& applicationSpecification)
    {
		s_Instance = this;
        m_Specifications = applicationSpecification;

        Log::Init(m_Specifications.Name);
        LUMINA_LOG_INFO("Starting Lumina Application: {}", m_Specifications.Name);
        
        glfwSetErrorCallback(GLFWErrorCallback);

        if (!glfwInit())
        {
            LUMINA_LOG_ERROR("GLFW failed to initialize.");
            return;
        }

        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

        m_Window = glfwCreateWindow(m_Specifications.Width, m_Specifications.Height, m_Specifications.Name.c_str(), NULL, NULL);
        if (!m_Window)
        {
            LUMINA_LOG_ERROR("Failed to create GLFW window.");
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSwapInterval(0);

        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        LUMINA_ASSERT(status, "[OpenGL Context] Failed to initialize GLAD.");

        const char* version = (const char*)glGetString(GL_VERSION);
        LUMINA_ASSERT(version, "[OpenGL Context] Failed to retrieve OpenGL version.");
        LUMINA_LOG_INFO("OpenGL Version: {}", version);

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

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);
 
        // Fullscreen with taskbar
        if (m_Specifications.Dock)
        {
            GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
            LUMINA_ASSERT(primaryMonitor, "Failed to get primary monitor.");
            if (primaryMonitor)
            {
                int xpos, ypos, width, height;
                glfwGetMonitorWorkarea(primaryMonitor, &xpos, &ypos, &width, &height);

                glfwSetWindowPos(m_Window, xpos, ypos);
                glfwSetWindowSize(m_Window, width, height);
            }
        }

        // Fullscreen
        if (m_Specifications.Fullscreen)
        {
            SetWindowFullscreen();
        }

        // Apply Theme
        if (m_Specifications.Theme)
            ApplyLuminaTheme(); 
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

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Application::Run()
    {
        while (!glfwWindowShouldClose(m_Window) && m_Running)
        {
            m_TimeStep = m_FrameTimer.Elapsed();
            m_FrameTimer.Reset();

            for (auto& layer : m_LayerStack)
                layer->OnUpdate(m_TimeStep);

            glfwPollEvents();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            RenderCustomTitleBar();

            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 dockspace_pos = viewport->WorkPos;
            ImVec2 dockspace_size = viewport->WorkSize;

            dockspace_pos.y += 30.0f;
            dockspace_size.y -= 30.0f;

            ImGui::SetNextWindowPos(dockspace_pos);
            ImGui::SetNextWindowSize(dockspace_size);
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

            // Create the dockspace
            ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

            ImGui::End();
            ImGui::PopStyleVar(3);

            // Layer from LuminaApp 
            for (auto& layer : m_LayerStack)
                layer->OnUIRender();

            // Render ImGui
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
                glfwSwapBuffers(m_Window);
        }
    }

    void Application::SetWindowFullscreen()
    {
        if (m_Specifications.Fullscreen)
        {
            glfwGetWindowPos(m_Window, &m_Specifications.PositionX, &m_Specifications.PositionY);
            glfwGetWindowSize(m_Window, (int*)&m_Specifications.Width, (int*)&m_Specifications.Height);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            LUMINA_ASSERT(monitor, "Failed to get primary monitor.");
            if (!monitor) return;

            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            LUMINA_ASSERT(mode, "Failed to get monitor video mode.");

            glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            glfwSetWindowMonitor(m_Window, nullptr, m_Specifications.PositionX, m_Specifications.PositionY,
                m_Specifications.Width, m_Specifications.Height, 0);
        }
    }

    void Application::RenderCustomTitleBar()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 30.0f));

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 0.0f)); // Remove vertical padding
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 0.0f));   // Reduce item spacing
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f)); // Center text in buttons

        if (ImGui::Begin("TopBox", nullptr, flags))
        {
            float windowHeight = ImGui::GetWindowHeight();
            float buttonHeight = 20.0f;
            float buttonVerticalOffset = (windowHeight - buttonHeight) * 0.5f;

            ImVec2 textSize = ImGui::CalcTextSize(m_Specifications.Name.c_str());
            float verticalOffset = (windowHeight - textSize.y) * 0.5f;

            ImGui::SetCursorPosY(verticalOffset);
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.2f, 1.0f));
            ImGui::Text("%s", m_Specifications.Name.c_str());
            ImGui::PopStyleColor();

            ImGui::SetCursorPos(ImVec2(0, 0));
            ImGui::InvisibleButton("##titlebar_drag", ImVec2(ImGui::GetWindowWidth() - 90.0f, 30.0f));

            if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    m_IsDragging = true;
                    ImVec2 mousePos = ImGui::GetMousePos();
                    int windowX, windowY;
                    glfwGetWindowPos(m_Window, &windowX, &windowY);
                    m_DragOffset = ImVec2(mousePos.x - windowX, mousePos.y - windowY);
                }
            }

            if (m_IsDragging)
            {
                if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
                {
                    ImVec2 mousePos = ImGui::GetMousePos();
                    ImVec2 newWindowPos = ImVec2(mousePos.x - m_DragOffset.x, mousePos.y - m_DragOffset.y);
                    glfwSetWindowPos(m_Window, (int)newWindowPos.x, (int)newWindowPos.y);
                }
                else
                {
                    m_IsDragging = false;
                }
            }

            float buttonsStartX = ImGui::GetWindowWidth() - 90.0f;
            ImGui::SetCursorPos(ImVec2(buttonsStartX, buttonVerticalOffset));

            if (ImGui::Button("_", ImVec2(25, 20)))
            {
                glfwIconifyWindow(m_Window);
            }

            ImGui::SameLine();

            if (ImGui::Button(m_Specifications.Dock ? "#" : "*", ImVec2(25, 20)))
            {
                ToggleDock();
            }

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            if (ImGui::Button("X", ImVec2(25, 20)))
            {
                m_Running = false;
            }
            ImGui::PopStyleColor(2);
        }

        ImGui::End();
        ImGui::PopStyleVar(3);
    }

    void Application::ToggleDock()
    {
        m_Specifications.Dock = !m_Specifications.Dock;

        if (m_Specifications.Dock)
        {
            if (!m_Specifications.Fullscreen)
            {
                glfwGetWindowPos(m_Window, &m_Specifications.PositionX, &m_Specifications.PositionY);
                glfwGetWindowSize(m_Window, (int*)&m_Specifications.Width, (int*)&m_Specifications.Height);
            }

            GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
            if (primaryMonitor)
            {
                int xpos, ypos, width, height;
                glfwGetMonitorWorkarea(primaryMonitor, &xpos, &ypos, &width, &height);
                glfwSetWindowPos(m_Window, xpos, ypos);
                glfwSetWindowSize(m_Window, width, height);
            }
        }
        else
        {
            glfwSetWindowSize(m_Window, m_Specifications.Width, m_Specifications.Height);
            glfwSetWindowPos(m_Window, m_Specifications.PositionX, m_Specifications.PositionY);
        }
    }

    void Application::ApplyLuminaTheme()
    {
        ImGui::StyleColorsDark();                                               // First apply dark theme 

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.0f;                                            // Rounded corners for windows
        style.FrameRounding = 5.0f;                                             // Rounded corners for frames
        style.FramePadding = ImVec2(5.0f, 5.0f);                                // Padding within a frame
        style.ItemSpacing = ImVec2(8.0f, 6.0f);                                 // Spacing between items

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);             // Dark background
        colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);              // Frame background
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);       // Hovered frame background
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);        // Active frame background
        colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);               // Button
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);        // Hovered button
        colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);         // Active button
        colors[ImGuiCol_Header] = ImVec4(0.8f, 0.4f, 0.1f, 0.8f);               // Header (Orange)
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.9f, 0.5f, 0.2f, 0.8f);        // Hovered header
        colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.6f, 0.3f, 1.0f);         // Active header
        colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);            // Checkmark (Orange)
        colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);           // Slider grab
        colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);     // Active slider grab
        colors[ImGuiCol_Tab] = ImVec4(0.8f, 0.4f, 0.1f, 1.0f);                  // Tab (Orange)
        colors[ImGuiCol_TabHovered] = ImVec4(0.9f, 0.5f, 0.2f, 1.0f);           // Hovered tab
        colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.6f, 0.3f, 1.0f);            // Active tab
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);         // Unfocused tab
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);   // Unfocused active tab

        // Maintain compatibility with multiple viewports
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;                                        // Disable rounding for additional platform windows
            colors[ImGuiCol_WindowBg].w = 1.0f;                                 // Fully opaque background
        }
    }
}