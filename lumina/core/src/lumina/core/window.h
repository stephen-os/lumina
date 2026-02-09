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
        std::string icon_path = "";
        uint32_t width = 1600;
        uint32_t height = 900;
        int32_t position_x = 100;
        int32_t position_y = 100;
        bool fullscreen = false;
        bool maximized = false;
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
        bool should_close() const;

        void set_event_callback(const event_callback& callback) { m_event_callback = callback; }

        void set_vsync(bool enabled);
        void set_fullscreen(bool fullscreen);
        void set_title(const std::string& title);
        void set_icon(const std::string& icon_path);
        void set_titlebar_color(uint8_t r, uint8_t g, uint8_t b);
        void set_titlebar_text_color(uint8_t r, uint8_t g, uint8_t b);
        void set_position(int32_t x, int32_t y);
        void maximize();

        uint32_t get_width() const { return m_spec.width; }
        uint32_t get_height() const { return m_spec.height; }
        bool is_vsync() const { return m_spec.vsync; }
        bool is_fullscreen() const { return m_spec.fullscreen; }

        GLFWwindow* get_native_window() const { return m_window; }

        static void terminate_glfw();

    private:
        void setup_callbacks();
        void set_default_icon();

        GLFWwindow* m_window = nullptr;
        window_spec m_spec;
        event_callback m_event_callback;
    };
}
