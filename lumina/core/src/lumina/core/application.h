#pragma once

#include "base.h"
#include "window.h"
#include "layer.h"
#include "event.h"
#include "device.h"
#include "input.h"

#include <string>
#include <vector>
#include <memory>

namespace lumina::core
{
    struct application_specifications
    {
        std::string title = "Lumina Application";
        std::string log_name = "Lumina";
        graphics_api api = graphics_api::vulkan;
    };

    class application
    {
    public:
        application(application_specifications specifications);
        ~application();

        application(const application&) = delete;
        application& operator=(const application&) = delete;

        void run();
        void shutdown() { m_running = false; }

        template<typename T>
        void push_layer()
        {
            static_assert(std::is_base_of_v<layer, T>, "T must derive from layer");
            m_layer_stack.emplace_back(std::make_shared<T>())->on_attach();
        }

        void push_layer(ref<layer> layer_ptr)
        {
            m_layer_stack.push_back(layer_ptr);
            layer_ptr->on_attach();
        }

        void on_event(event& e);
        void post_event(event& e);
        void queue_event(std::unique_ptr<event> e);

        // Window configuration
        void set_title(const std::string& title);
        void set_icon(const std::string& icon_path);
        void set_titlebar_color(uint8_t r, uint8_t g, uint8_t b);
        void set_titlebar_text_color(uint8_t r, uint8_t g, uint8_t b);
        void set_fullscreen(bool fullscreen);
        void set_vsync(bool enabled);
        void set_position(int32_t x, int32_t y);
        void maximize();

        // Window state
        [[nodiscard]] uint32_t get_width() const { return m_window->get_width(); }
        [[nodiscard]] uint32_t get_height() const { return m_window->get_height(); }
        [[nodiscard]] bool is_fullscreen() const { return m_window->is_fullscreen(); }
        [[nodiscard]] bool is_vsync() const { return m_window->is_vsync(); }

        // Device access
        [[nodiscard]] device& get_device() { return *m_device; }
        [[nodiscard]] nvrhi::IDevice* get_nvrhi_device() { return m_device->get_nvrhi_device(); }

        [[nodiscard]] static application& get();
        [[nodiscard]] static float get_time();

    private:
        void init_imgui();
        void shutdown_imgui();
        void begin_frame();
        void end_frame();
        void on_resize(uint32_t width, uint32_t height);

        // Internal window access for input system
        [[nodiscard]] GLFWwindow* get_window_handle() const { return m_window->get_native_window(); }
        friend bool input::is_key_pressed(input::key_code);
        friend bool input::is_mouse_button_pressed(input::mouse_code);
        friend void input::set_cursor_mode(input::cursor_mode);
        friend input::mouse_position input::get_mouse_position();

		std::string m_title;
        graphics_api m_api;
        
        scope<window> m_window;
        scope<device> m_device;

        bool m_running = true;
        std::vector<ref<layer>> m_layer_stack;
        std::vector<std::unique_ptr<event>> m_event_queue;
    };

    // User must implement this function
    application* create_application(int argc, char** argv);
}
