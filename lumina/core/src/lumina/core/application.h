#pragma once

#include "base.h"
#include "window.h"
#include "layer.h"
#include "event.h"
#include "device.h"
#include "input.h"

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <optional>

namespace lumina::core
{
    enum class window_start_mode
    {
        normal,     // Default position
        centered,   // Center on primary monitor
        maximized   // Start maximized
    };

    struct application_specifications
    {
        // Identity (immutable after creation)
        std::string title = "Lumina Application";
        std::filesystem::path icon_path;
        std::filesystem::path working_directory;

        // Window configuration
        uint32_t width = 1600;
        uint32_t height = 900;
        window_start_mode start_mode = window_start_mode::centered;
        bool fullscreen = false;
        bool resizable = true;
        bool decorated = true;

        // Rendering
        bool vsync = true;
        bool enable_imgui = true;
        graphics_api api = graphics_api::vulkan;
        glm::vec4 clear_color = {0.1f, 0.1f, 0.1f, 1.0f};

        // Optional titlebar theming (Windows only)
        struct titlebar_theme
        {
            glm::vec3 background = {0.118f, 0.118f, 0.118f};
            glm::vec3 text = {1.0f, 1.0f, 1.0f};
        };
        std::optional<titlebar_theme> titlebar;
    };

    class application
    {
    public:
        application(const application_specifications& specifications);
        ~application();

        application(const application&) = delete;
        application& operator=(const application&) = delete;

        void run();
        void shutdown() { m_running = false; }

        // Layer management
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

        // Events
        void on_event(event& e);
        void post_event(event& e);
        void queue_event(std::unique_ptr<event> e);

        // Runtime window operations
        void set_fullscreen(bool fullscreen);
        void set_vsync(bool enabled);
        void set_position(int32_t x, int32_t y);
        void maximize();
        void minimize();
        void restore();

        // Window state
        [[nodiscard]] uint32_t get_width() const;
        [[nodiscard]] uint32_t get_height() const;
        [[nodiscard]] bool is_fullscreen() const;
        [[nodiscard]] bool is_vsync() const;
        [[nodiscard]] bool is_maximized() const;
        [[nodiscard]] bool is_minimized() const;

        // Device access
        [[nodiscard]] device& get_device() { return *m_device; }
        [[nodiscard]] nvrhi::IDevice* get_nvrhi_device() { return m_device->get_nvrhi_device(); }

        // Specifications
        [[nodiscard]] const application_specifications& get_specifications() const { return m_specs; }

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

        application_specifications m_specs;
        scope<window> m_window;
        scope<device> m_device;

        bool m_running = true;
        std::vector<ref<layer>> m_layer_stack;
        std::vector<std::unique_ptr<event>> m_event_queue;
    };

    // User must implement this function
    application* create_application(int argc, char** argv);
}
