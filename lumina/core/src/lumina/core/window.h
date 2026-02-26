#pragma once

#include "event.h"

#include <string>
#include <functional>

struct GLFWwindow;

namespace lumina::core
{
    struct window_spec
    {
        std::string title = "Lumina Application";
        std::string icon_path;
        uint32_t width = 1600;
        uint32_t height = 900;
        bool fullscreen = false;
        bool maximized = false;
        bool centered = false;
        bool resizable = true;
        bool decorated = true;
        bool vsync = true;
    };

    class window
    {
    public:
        using event_callback = std::function<void(event&)>;

        window(const window_spec& spec);
        ~window();

        window(const window&) = delete;
        window& operator=(const window&) = delete;

        void update();
        [[nodiscard]] bool should_close() const;

        void set_event_callback(const event_callback& callback) { m_event_callback = callback; }

        // Runtime operations
        void set_vsync(bool enabled);
        void set_fullscreen(bool fullscreen);
        void set_position(int32_t x, int32_t y);
        void maximize();
        void minimize();
        void restore();
        void show();
        void center_on_monitor();

        // Titlebar theming (Windows only)
        void set_titlebar_color(uint8_t r, uint8_t g, uint8_t b);
        void set_titlebar_text_color(uint8_t r, uint8_t g, uint8_t b);

        // State queries
        [[nodiscard]] uint32_t get_width() const { return m_width; }
        [[nodiscard]] uint32_t get_height() const { return m_height; }
        [[nodiscard]] bool is_vsync() const { return m_vsync; }
        [[nodiscard]] bool is_fullscreen() const { return m_fullscreen; }
        [[nodiscard]] bool is_maximized() const;
        [[nodiscard]] bool is_minimized() const;

        [[nodiscard]] GLFWwindow* get_native_window() const { return m_window; }

        static void terminate_glfw();

    private:
        void setup_callbacks();
        void set_icon(const std::string& icon_path);
        void set_default_icon();

        GLFWwindow* m_window = nullptr;
        event_callback m_event_callback;

        // Cached state
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        int32_t m_windowed_x = 100;
        int32_t m_windowed_y = 100;
        uint32_t m_windowed_width = 0;
        uint32_t m_windowed_height = 0;
        bool m_vsync = true;
        bool m_fullscreen = false;
    };
}
