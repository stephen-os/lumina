#include "Window.h"
#include "Log.h"
#include "Assert.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"

#include <stb/stb_image.h>
#include <glad/glad.h>

namespace Lumina
{
    static bool s_GLFWInitialized = false;

    static void GLFWErrorCallback(int error, const char* description)
    {
        LUMINA_LOG_ERROR("[GLFW ERROR] {}: {}", error, description);
    }

    Window::Window(const WindowSpecification& specification) 
    {
        m_WindowSpecifications = specification;

        if (!s_GLFWInitialized)
        {
            glfwSetErrorCallback(GLFWErrorCallback);

            if (!glfwInit())
            {
                LUMINA_LOG_ERROR("GLFW failed to initialize.");
                return;
            }
            s_GLFWInitialized = true;
        }

        m_Window = glfwCreateWindow(m_WindowSpecifications.Width, m_WindowSpecifications.Height, m_WindowSpecifications.Title.c_str(), nullptr, nullptr);

        if (!m_Window)
        {
            LUMINA_LOG_ERROR("Failed to create GLFW window.");
            return;
        }

        glfwMakeContextCurrent(m_Window);
        SetVSync(m_WindowSpecifications.VSync);

        // Set window title bar colors
        glfwSetWindowTitleBarColor(m_Window, 45, 45, 45);
        glfwSetWindowTitleBarTextColor(m_Window, 255, 153, 51);

        // Set window icon if specified
        if (!specification.IconPath.empty())
        {
            SetIcon(specification.IconPath);
        }

        // Initialize GLAD
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        LUMINA_ASSERT(status, "[OpenGL Context] Failed to initialize GLAD.");

        const char* version = (const char*)glGetString(GL_VERSION);
        LUMINA_ASSERT(version, "[OpenGL Context] Failed to retrieve OpenGL version.");
        LUMINA_LOG_INFO("OpenGL Version: {}", version);

		SetupCallbacks();

        // Apply fullscreen or maximized state
        if (specification.Fullscreen)
        {
            SetFullscreen(true);
        }
        else if (specification.Maximized)
        {
            Maximize();
        }
    }

    Window::~Window()
    {
        if (m_Window)
        {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
    }

    void Window::SetupCallbacks()
    {
        glfwSetWindowUserPointer(m_Window, this);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
            {
                Window* win = (Window*)glfwGetWindowUserPointer(window);
                win->m_WindowSpecifications.Width = width;
                win->m_WindowSpecifications.Height = height;

                if (win->m_WindowEventCallback)
                {
                    WindowResizeEvent event(width, height);
                    win->m_WindowEventCallback(event);
                }
            });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
            {
                Window* win = (Window*)glfwGetWindowUserPointer(window);
                if (win->m_WindowEventCallback)
                {
                    WindowCloseEvent event;
                    win->m_WindowEventCallback(event);
                }
            });

        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focused)
            {
                Window* win = (Window*)glfwGetWindowUserPointer(window);
                if (win->m_WindowEventCallback)
                {
                    if (focused)
                    {
                        WindowFocusEvent event;
                        win->m_WindowEventCallback(event);
                    }
                    else
                    {
                        WindowLostFocusEvent event;
                        win->m_WindowEventCallback(event);
                    }
                }
            });

        glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
            {
                Window* win = (Window*)glfwGetWindowUserPointer(window);
                if (win->m_WindowEventCallback)
                {
                    WindowMovedEvent event(xpos, ypos);
                    win->m_WindowEventCallback(event);
                }
            });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                Window* win = (Window*)glfwGetWindowUserPointer(window);
                if (win->m_WindowEventCallback)
                {
                    switch (action)
                    {
                    case GLFW_PRESS:
                    {
                        KeyPressedEvent event(key, false);
                        win->m_WindowEventCallback(event);
                        break;
                    }
                    case GLFW_RELEASE:
                    {
                        KeyReleasedEvent event(key);
                        win->m_WindowEventCallback(event);
                        break;
                    }
                    case GLFW_REPEAT:
                    {
                        KeyPressedEvent event(key, true);
                        win->m_WindowEventCallback(event);
                        break;
                    }
                    }
                }
            });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int codepoint)
            {
                Window* win = (Window*)glfwGetWindowUserPointer(window);
                if (win->m_WindowEventCallback)
                {
                    KeyTypedEvent event(codepoint);
                    win->m_WindowEventCallback(event);
                }
            });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
            {
                Window* win = (Window*)glfwGetWindowUserPointer(window);
                if (win->m_WindowEventCallback)
                {
                    switch (action)
                    {
                    case GLFW_PRESS:
                    {
                        MouseButtonPressedEvent event(button);
                        win->m_WindowEventCallback(event);
                        break;
                    }
                    case GLFW_RELEASE:
                    {
                        MouseButtonReleasedEvent event(button);
                        win->m_WindowEventCallback(event);
                        break;
                    }
                    }
                }
            });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
            {
                Window* win = (Window*)glfwGetWindowUserPointer(window);
                if (win->m_WindowEventCallback)
                {
                    MouseScrolledEvent event((float)xOffset, (float)yOffset);
                    win->m_WindowEventCallback(event);
                }
            });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
            {
                Window* win = (Window*)glfwGetWindowUserPointer(window);
                if (win->m_WindowEventCallback)
                {
                    MouseMovedEvent event((float)xPos, (float)yPos);
                    win->m_WindowEventCallback(event);
                }
            });
    }

    void Window::Update()
    {
        glfwPollEvents();
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    bool Window::ShouldClose() const
    {
        return glfwWindowShouldClose(m_Window);
    }

    void Window::SetVSync(bool enabled)
    {
        glfwSwapInterval(enabled ? 1 : 0);
        m_WindowSpecifications.VSync = enabled;
    }

    void Window::SetFullscreen(bool fullscreen)
    {
        if (m_WindowSpecifications.Fullscreen == fullscreen)
            return;

        if (fullscreen)
        {
            // Store window position and size before going fullscreen
            glfwGetWindowPos(m_Window, &m_WindowSpecifications.PositionX, &m_WindowSpecifications.PositionY);
            glfwGetWindowSize(m_Window, (int*)&m_WindowSpecifications.Width, (int*)&m_WindowSpecifications.Height);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            LUMINA_ASSERT(monitor, "Failed to get primary monitor.");
            if (!monitor) return;

            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            LUMINA_ASSERT(mode, "Failed to get monitor video mode.");

            glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            glfwSetWindowMonitor(m_Window, nullptr, m_WindowSpecifications.PositionX, m_WindowSpecifications.PositionY,
                m_WindowSpecifications.Width, m_WindowSpecifications.Height, 0);
        }

        m_WindowSpecifications.Fullscreen = fullscreen;
    }

    void Window::Maximize()
    {
		m_WindowSpecifications.Maximized = true;
        glfwMaximizeWindow(m_Window);
    }

    void Window::SetIcon(const std::string& iconPath)
    {
        GLFWimage icon;
        icon.pixels = stbi_load(iconPath.c_str(), &icon.width, &icon.height, 0, 4);
        if (icon.pixels)
        {
            m_WindowSpecifications.IconPath = iconPath;

            glfwSetWindowIcon(m_Window, 1, &icon);
            stbi_image_free(icon.pixels);
        }
        else
        {
            LUMINA_LOG_WARN("Failed to load window icon: {}", iconPath);
        }
    }

    void Window::SetTitle(const std::string& title)
    {
        m_WindowSpecifications.Title = title;
        glfwSetWindowTitle(m_Window, title.c_str());
    }
}