#pragma once

#include <Lumina/Events/Event.h>

#include <string>
#include <functional>

#include <GLFW/glfw3.h>

namespace Lumina
{
    struct WindowSpecification
    {
        std::string Title = "Lumina Window";
        std::string IconPath = "";
        uint32_t Width = 1600;
        uint32_t Height = 900;
        int32_t PositionX = 100;
        int32_t PositionY = 100;
        bool Fullscreen = false;
        bool Maximized = false;
        bool VSync = true;
    };

    class Window
    {
    public:
        Window(const WindowSpecification& specification = WindowSpecification());
        ~Window();

        void Update();
        void SwapBuffers();
        bool ShouldClose() const;

        void SetVSync(bool enabled);
        void SetFullscreen(bool fullscreen);
        void Maximize();
        void SetIcon(const std::string& iconPath);
        void SetTitle(const std::string& title);

        uint32_t GetWidth() const { return m_WindowSpecifications.Width; }
        uint32_t GetHeight() const { return m_WindowSpecifications.Height; }
        GLFWwindow* GetNativeWindow() const { return m_Window; }

        void SetEventCallback(const std::function<void(class Event&)>& callback) { m_WindowEventCallback = callback; }

    private: 
        void SetupCallbacks();

    private: 
        std::function<void(class Event&)> m_WindowEventCallback;
        WindowSpecification m_WindowSpecifications;

        GLFWwindow* m_Window = nullptr;
    };
}