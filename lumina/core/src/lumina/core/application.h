#pragma once

#include "base.h"
#include "window.h"
#include "layer.h"
#include "event.h"
#include "graphics_device.h"

#include <string>
#include <vector>
#include <memory>

struct GLFWwindow;

namespace lumina::core
{
    struct application_spec
    {
        std::string name = "Lumina Application";
        std::string icon = "";
        uint32_t width = 1600;
        uint32_t height = 900;
        int32_t position_x = 100;
        int32_t position_y = 100;
        bool fullscreen = false;
        bool maximized = false;
        bool enable_docking = true;
        bool dark_theme = true;
        bool vsync = true;
        graphics_api api = graphics_api::vulkan;
    };

    class application
    {
    public:
        application(const application_spec& spec = application_spec());
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

        void set_fullscreen();

        uint32_t get_width() const { return m_window->get_width(); }
        uint32_t get_height() const { return m_window->get_height(); }
        GLFWwindow* get_window_handle() const { return m_window->get_native_window(); }
        window& get_window() { return *m_window; }

        graphics_device& get_graphics_device() { return *m_graphics_device; }
        nvrhi::IDevice* get_nvrhi_device() { return m_graphics_device->get_device(); }

        static application& get();
        static float get_time();

    private:
        void init_imgui();
        void shutdown_imgui();
        void begin_frame();
        void end_frame();
        void on_resize(uint32_t width, uint32_t height);

        application_spec m_spec;
        scope<window> m_window;
        scope<graphics_device> m_graphics_device;

        bool m_running = true;
        std::vector<ref<layer>> m_layer_stack;
        std::vector<std::unique_ptr<event>> m_event_queue;
    };

    // User must implement this function
    application* create_application(int argc, char** argv);
}
