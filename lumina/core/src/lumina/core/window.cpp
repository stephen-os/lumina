#include "window.h"
#include "log.h"
#include "assert.h"
#include "input.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace lumina::core
{
    static bool s_glfw_initialized = false;

    static void glfw_error_callback(int error, const char* description)
    {
        LUMINA_LOG_ERROR("[GLFW] Error {}: {}", error, description);
    }

    window::window(const window_spec& spec)
        : m_spec(spec)
    {
        if (!s_glfw_initialized)
        {
            glfwSetErrorCallback(glfw_error_callback);

            if (!glfwInit())
            {
                LUMINA_LOG_ERROR("Failed to initialize GLFW");
                return;
            }
            s_glfw_initialized = true;
        }

        // No graphics API - NVRHI will handle this
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_window = glfwCreateWindow(
            static_cast<int>(m_spec.width),
            static_cast<int>(m_spec.height),
            m_spec.title.c_str(),
            nullptr,
            nullptr
        );

        if (!m_window)
        {
            LUMINA_LOG_ERROR("Failed to create GLFW window");
            return;
        }

        // Position window
        glfwSetWindowPos(m_window, m_spec.position_x, m_spec.position_y);

        // Set window icon if specified
        if (!m_spec.icon_path.empty())
        {
            set_icon(m_spec.icon_path);
        }

        setup_callbacks();

        // Apply fullscreen or maximized state
        if (m_spec.fullscreen)
        {
            set_fullscreen(true);
        }
        else if (m_spec.maximized)
        {
            maximize();
        }

        LUMINA_LOG_INFO("Window created: {}x{}", m_spec.width, m_spec.height);
    }

    window::~window()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }

        glfwTerminate();
        s_glfw_initialized = false;
    }

    void window::setup_callbacks()
    {
        glfwSetWindowUserPointer(m_window, this);

        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* glfw_window, int width, int height)
        {
            window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
            win->m_spec.width = static_cast<uint32_t>(width);
            win->m_spec.height = static_cast<uint32_t>(height);

            if (win->m_event_callback)
            {
                window_resize_event e(width, height);
                win->m_event_callback(e);
            }
        });

        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* glfw_window)
        {
            window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
            if (win->m_event_callback)
            {
                window_close_event e;
                win->m_event_callback(e);
            }
        });

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* glfw_window, int focused)
        {
            window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
            if (win->m_event_callback)
            {
                if (focused)
                {
                    window_focus_event e;
                    win->m_event_callback(e);
                }
                else
                {
                    window_lost_focus_event e;
                    win->m_event_callback(e);
                }
            }
        });

        glfwSetWindowPosCallback(m_window, [](GLFWwindow* glfw_window, int xpos, int ypos)
        {
            window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
            if (win->m_event_callback)
            {
                window_moved_event e(xpos, ypos);
                win->m_event_callback(e);
            }
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
        {
            window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
            if (win->m_event_callback)
            {
                input::key_code keycode = static_cast<input::key_code>(key);
                switch (action)
                {
                case GLFW_PRESS:
                {
                    key_pressed_event e(keycode, false);
                    win->m_event_callback(e);
                    break;
                }
                case GLFW_RELEASE:
                {
                    key_released_event e(keycode);
                    win->m_event_callback(e);
                    break;
                }
                case GLFW_REPEAT:
                {
                    key_pressed_event e(keycode, true);
                    win->m_event_callback(e);
                    break;
                }
                }
            }
        });

        glfwSetCharCallback(m_window, [](GLFWwindow* glfw_window, unsigned int codepoint)
        {
            window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
            if (win->m_event_callback)
            {
                key_typed_event e(codepoint);
                win->m_event_callback(e);
            }
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* glfw_window, int button, int action, int mods)
        {
            window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
            if (win->m_event_callback)
            {
                input::mouse_code mouse_button = static_cast<input::mouse_code>(button);
                switch (action)
                {
                case GLFW_PRESS:
                {
                    mouse_button_pressed_event e(mouse_button);
                    win->m_event_callback(e);
                    break;
                }
                case GLFW_RELEASE:
                {
                    mouse_button_released_event e(mouse_button);
                    win->m_event_callback(e);
                    break;
                }
                }
            }
        });

        glfwSetScrollCallback(m_window, [](GLFWwindow* glfw_window, double x_offset, double y_offset)
        {
            window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
            if (win->m_event_callback)
            {
                mouse_scrolled_event e(static_cast<float>(x_offset), static_cast<float>(y_offset));
                win->m_event_callback(e);
            }
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* glfw_window, double xpos, double ypos)
        {
            window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
            if (win->m_event_callback)
            {
                mouse_moved_event e(static_cast<float>(xpos), static_cast<float>(ypos));
                win->m_event_callback(e);
            }
        });
    }

    void window::update()
    {
        glfwPollEvents();
    }

    bool window::should_close() const
    {
        return glfwWindowShouldClose(m_window);
    }

    void window::set_vsync(bool enabled)
    {
        // VSync is handled by NVRHI swapchain, not GLFW
        m_spec.vsync = enabled;
    }

    void window::set_fullscreen(bool fullscreen)
    {
        if (m_spec.fullscreen == fullscreen)
            return;

        if (fullscreen)
        {
            // Store window position and size before going fullscreen
            glfwGetWindowPos(m_window, &m_spec.position_x, &m_spec.position_y);
            glfwGetWindowSize(m_window, reinterpret_cast<int*>(&m_spec.width), reinterpret_cast<int*>(&m_spec.height));

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            LUMINA_ASSERT(monitor, "Failed to get primary monitor");
            if (!monitor) return;

            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            LUMINA_ASSERT(mode, "Failed to get monitor video mode");

            glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            glfwSetWindowMonitor(m_window, nullptr,
                m_spec.position_x, m_spec.position_y,
                m_spec.width, m_spec.height, 0);
        }

        m_spec.fullscreen = fullscreen;
    }

    void window::maximize()
    {
        m_spec.maximized = true;
        glfwMaximizeWindow(m_window);
    }

    void window::set_icon(const std::string& icon_path)
    {
        // TODO: Implement icon loading with stb_image
        // For now, just store the path
        m_spec.icon_path = icon_path;
        LUMINA_LOG_WARN("Window icon loading not yet implemented");
    }

    void window::set_title(const std::string& title)
    {
        m_spec.title = title;
        glfwSetWindowTitle(m_window, title.c_str());
    }
}
