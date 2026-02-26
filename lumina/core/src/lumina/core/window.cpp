#include "window.h"
#include "log.h"
#include "assert.h"
#include "input.h"
#include "default_icon.h"

#include <GLFW/glfw3.h>

#ifdef LUMINA_PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
    #include <dwmapi.h>

    #ifndef DWMWA_CAPTION_COLOR
        #define DWMWA_CAPTION_COLOR 35
    #endif
    #ifndef DWMWA_TEXT_COLOR
        #define DWMWA_TEXT_COLOR 36
    #endif
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

namespace lumina::core
{
    static bool s_glfw_initialized = false;

    static void glfw_error_callback(int error, const char* description)
    {
        LUMINA_LOG_ERROR("[GLFW] Error {}: {}", error, description);
    }

    window::window(const window_spec& spec)
        : m_width(spec.width)
        , m_height(spec.height)
        , m_windowed_width(spec.width)
        , m_windowed_height(spec.height)
        , m_vsync(spec.vsync)
        , m_fullscreen(spec.fullscreen)
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

        // Window hints
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_RESIZABLE, spec.resizable ? GLFW_TRUE : GLFW_FALSE);
        glfwWindowHint(GLFW_DECORATED, spec.decorated ? GLFW_TRUE : GLFW_FALSE);

        m_window = glfwCreateWindow(
            static_cast<int>(spec.width),
            static_cast<int>(spec.height),
            spec.title.c_str(),
            nullptr,
            nullptr
        );

        if (!m_window)
        {
            LUMINA_LOG_ERROR("Failed to create GLFW window");
            return;
        }

        // Set window icon (custom or default)
        if (!spec.icon_path.empty())
        {
            set_icon(spec.icon_path);
        }
        else
        {
            set_default_icon();
        }

        setup_callbacks();

        // Apply initial window state
        if (spec.fullscreen)
        {
            set_fullscreen(true);
        }
        else if (spec.maximized)
        {
            maximize();
        }
        else if (spec.centered)
        {
            center_on_monitor();
        }

        LUMINA_LOG_INFO("Window created: {}x{}", m_width, m_height);
    }

    window::~window()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
    }

    void window::terminate_glfw()
    {
        if (s_glfw_initialized)
        {
            glfwTerminate();
            s_glfw_initialized = false;
        }
    }

    void window::setup_callbacks()
    {
        glfwSetWindowUserPointer(m_window, this);

        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* glfw_window, int width, int height)
        {
            window* win = static_cast<window*>(glfwGetWindowUserPointer(glfw_window));
            win->m_width = static_cast<uint32_t>(width);
            win->m_height = static_cast<uint32_t>(height);

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
        m_vsync = enabled;
    }

    void window::set_fullscreen(bool fullscreen)
    {
        if (m_fullscreen == fullscreen)
            return;

        if (fullscreen)
        {
            // Store windowed position and size before going fullscreen
            int x, y;
            glfwGetWindowPos(m_window, &x, &y);
            m_windowed_x = x;
            m_windowed_y = y;
            m_windowed_width = m_width;
            m_windowed_height = m_height;

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
                m_windowed_x, m_windowed_y,
                static_cast<int>(m_windowed_width), static_cast<int>(m_windowed_height), 0);
        }

        m_fullscreen = fullscreen;
    }

    void window::set_position(int32_t x, int32_t y)
    {
        glfwSetWindowPos(m_window, x, y);
    }

    void window::maximize()
    {
        glfwMaximizeWindow(m_window);
    }

    void window::minimize()
    {
        glfwIconifyWindow(m_window);
    }

    void window::restore()
    {
        glfwRestoreWindow(m_window);
    }

    void window::show()
    {
        glfwShowWindow(m_window);
    }

    void window::center_on_monitor()
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (!monitor)
        {
            LUMINA_LOG_WARN("Failed to get primary monitor for centering");
            return;
        }

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (!mode)
        {
            LUMINA_LOG_WARN("Failed to get video mode for centering");
            return;
        }

        int monitor_x, monitor_y;
        glfwGetMonitorPos(monitor, &monitor_x, &monitor_y);

        int window_width, window_height;
        glfwGetWindowSize(m_window, &window_width, &window_height);

        int center_x = monitor_x + (mode->width - window_width) / 2;
        int center_y = monitor_y + (mode->height - window_height) / 2;

        glfwSetWindowPos(m_window, center_x, center_y);
        m_windowed_x = center_x;
        m_windowed_y = center_y;
    }

    bool window::is_maximized() const
    {
        return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED) == GLFW_TRUE;
    }

    bool window::is_minimized() const
    {
        return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) == GLFW_TRUE;
    }

    void window::set_icon(const std::string& icon_path)
    {
        int width, height, channels;
        unsigned char* pixels = stbi_load(icon_path.c_str(), &width, &height, &channels, 4);

        if (!pixels)
        {
            LUMINA_LOG_ERROR("Failed to load window icon: {}", icon_path);
            return;
        }

        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = pixels;

        glfwSetWindowIcon(m_window, 1, &icon);
        stbi_image_free(pixels);

        LUMINA_LOG_INFO("Window icon set: {} ({}x{})", icon_path, width, height);
    }

    void window::set_default_icon()
    {
        int width, height, channels;
        unsigned char* pixels = stbi_load_from_memory(
            default_icon_data,
            static_cast<int>(default_icon_size),
            &width, &height, &channels, 4
        );

        if (!pixels)
        {
            LUMINA_LOG_WARN("Failed to load default window icon");
            return;
        }

        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = pixels;

        glfwSetWindowIcon(m_window, 1, &icon);
        stbi_image_free(pixels);
    }

    void window::set_titlebar_color(uint8_t r, uint8_t g, uint8_t b)
    {
#ifdef LUMINA_PLATFORM_WINDOWS
        HWND hwnd = glfwGetWin32Window(m_window);
        COLORREF color = RGB(r, g, b);
        HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &color, sizeof(color));
        if (FAILED(hr))
        {
            LUMINA_LOG_WARN("Failed to set titlebar color (requires Windows 11+)");
        }
#else
        LUMINA_LOG_WARN("Titlebar color customization is only supported on Windows");
        (void)r; (void)g; (void)b;
#endif
    }

    void window::set_titlebar_text_color(uint8_t r, uint8_t g, uint8_t b)
    {
#ifdef LUMINA_PLATFORM_WINDOWS
        HWND hwnd = glfwGetWin32Window(m_window);
        COLORREF color = RGB(r, g, b);
        HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &color, sizeof(color));
        if (FAILED(hr))
        {
            LUMINA_LOG_WARN("Failed to set titlebar text color (requires Windows 11+)");
        }
#else
        LUMINA_LOG_WARN("Titlebar text color customization is only supported on Windows");
        (void)r; (void)g; (void)b;
#endif
    }
}
