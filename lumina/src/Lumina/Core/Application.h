#pragma once

#include "imgui.h"

#include "Layer.h"
#include "Window.h"

#include "Events/Event.h"

#include <string>
#include <vector>
#include <memory>

namespace Lumina
{
    struct ApplicationSpecification
    {
        std::string Name = "Lumina App";
        std::string Icon = "";
        uint32_t Width = 1600;
        uint32_t Height = 900;
        int32_t PositionX = 100;
        int32_t PositionY = 100;
		bool EnableDocking = true;
        bool Use2DRenderer = false;
        bool Use3DRenderer = false;
        bool Fullscreen = false;
        bool Maximized = false;
        bool Theme = true;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& applicationSpecification = ApplicationSpecification());
        ~Application();

        void Run();

        template<typename T>
        void PushLayer()
        {
            static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
            m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
        }

        void OnEvent(Event& e);
        void PostEvent(Event& e);
        void QueueEvent(std::unique_ptr<Event> e);

        void SetWindowFullscreen();
        void Shutdown() { m_Running = false; }

        ImGuiID GetDockspaceID() { return m_DockspaceID; }
        GLFWwindow* GetWindowHandle() const { return m_Window->GetNativeWindow(); }

        static Application& GetInstance();
        static float GetTime(); 

    private:
        ApplicationSpecification m_Specifications;
        
        std::unique_ptr<Window> m_Window;

        ImGuiID m_DockspaceID = 0;
        
        bool m_Running = true;
        std::vector<std::shared_ptr<Layer>> m_LayerStack;

        std::vector<std::unique_ptr<Event>> m_EventQueue;
    };

    Application* CreateApplication(int argc, char** argv);
}