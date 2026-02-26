#include "application.h"
#include "log.h"
#include "assert.h"
#include "theme.h"
#include "imgui/imgui_nvrhi.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

#include <nvrhi/utils.h>

#include <algorithm>

namespace lumina::core
{
    static application* s_instance = nullptr;

    application& application::get()
    {
        LUMINA_ASSERT(s_instance, "Application instance is null");
        return *s_instance;
    }

    application::application(const application_specifications& specifications)
        : m_specs(specifications)
    {
        // TODO: Different loggers for different subsystems
        // For example, we should have a core logger and a client logger.
        log::init("Lumina");

        LUMINA_ASSERT(!s_instance, "Application already exists");
        s_instance = this;

        // Set working directory if specified
        if (!m_specs.working_directory.empty())
        {
            std::filesystem::current_path(m_specs.working_directory);
        }

        LUMINA_LOG_INFO("Starting Lumina Application: {}", m_specs.title);

        // Build window spec from application specifications
        window_spec win_spec;
        win_spec.title = m_specs.title;
        win_spec.icon_path = m_specs.icon_path.string();
        win_spec.width = m_specs.width;
        win_spec.height = m_specs.height;
        win_spec.fullscreen = m_specs.fullscreen;
        win_spec.maximized = (m_specs.start_mode == window_start_mode::maximized);
        win_spec.centered = (m_specs.start_mode == window_start_mode::centered);
        win_spec.resizable = m_specs.resizable;
        win_spec.decorated = m_specs.decorated;
        win_spec.vsync = m_specs.vsync;

        m_window = make_scope<window>(win_spec);
        m_window->set_event_callback([this](event& e) { on_event(e); });
        LUMINA_ASSERT(m_window, "Failed to create application window");

        // Apply titlebar theme if specified (Windows only)
        if (m_specs.titlebar.has_value())
        {
            const auto& theme = m_specs.titlebar.value();
            m_window->set_titlebar_color(
                static_cast<uint8_t>(theme.background.r * 255.0f),
                static_cast<uint8_t>(theme.background.g * 255.0f),
                static_cast<uint8_t>(theme.background.b * 255.0f)
            );
            m_window->set_titlebar_text_color(
                static_cast<uint8_t>(theme.text.r * 255.0f),
                static_cast<uint8_t>(theme.text.g * 255.0f),
                static_cast<uint8_t>(theme.text.b * 255.0f)
            );
        }

        // Initialize device
        m_device = device::create(m_specs.api);
        LUMINA_ASSERT(m_device, "Failed to create device");

        device_desc dev_desc;
        dev_desc.window = m_window->get_native_window();
        dev_desc.width = m_window->get_width();
        dev_desc.height = m_window->get_height();
        dev_desc.vsync = m_specs.vsync;
        dev_desc.app_name = m_specs.title.c_str();
#ifdef LUMINA_DEBUG
        dev_desc.enable_debug_layer = true;
#endif

        bool dev_init = m_device->init(dev_desc);
        LUMINA_ASSERT(dev_init, "Failed to initialize device");

        if (m_specs.enable_imgui)
        {
            init_imgui();
        }
    }

    application::~application()
    {
        m_event_queue.clear();

        for (auto& layer : m_layer_stack)
            layer->on_detach();

        m_layer_stack.clear();

        if (m_specs.enable_imgui)
        {
            shutdown_imgui();
        }

        m_device->shutdown();
        m_device.reset();

        m_window.reset();
        window::terminate_glfw();

        log::shutdown();
        s_instance = nullptr;
    }

    void application::init_imgui()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Initialize GLFW backend for ImGui (handles input)
        ImGui_ImplGlfw_InitForOther(m_window->get_native_window(), true);

        // Initialize NVRHI backend for ImGui rendering
        imgui::imgui_nvrhi_config imgui_config;
        imgui_config.device = m_device->get_nvrhi_device();
        imgui_config.render_target_format = m_device->get_swapchain_format();

        bool imgui_init = imgui::init(imgui_config);
        LUMINA_ASSERT(imgui_init, "Failed to initialize ImGui NVRHI backend");

        imgui::init_platform_viewports(*m_device);

        theme::apply_lumina_theme();

        LUMINA_LOG_INFO("ImGui initialized");
    }

    void application::shutdown_imgui()
    {
        imgui::shutdown_platform_viewports();
        imgui::shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void application::begin_frame()
    {
        m_device->begin_frame();

        // Clear the framebuffer
        auto cmd = m_device->get_command_list();
        auto fb = m_device->get_current_framebuffer();

        nvrhi::utils::ClearColorAttachment(cmd, fb, 0, nvrhi::Color(
            m_specs.clear_color.r,
            m_specs.clear_color.g,
            m_specs.clear_color.b,
            m_specs.clear_color.a
        ));

        if (m_specs.enable_imgui)
        {
            imgui::new_frame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }
    }

    void application::end_frame()
    {
        if (m_specs.enable_imgui)
        {
            ImGui::Render();

            // Render ImGui draw data using NVRHI
            auto cmd = m_device->get_command_list();
            auto fb = m_device->get_current_framebuffer();
            imgui::render_draw_data(cmd, fb, ImGui::GetDrawData());
        }

        // Present the main window first — this closes and executes the main command list.
        // Viewport rendering must happen after, since NVRHI only allows one immediate
        // command list open at a time.
        m_device->present();

        if (m_specs.enable_imgui)
        {
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
        }
    }

    void application::on_event(event& e)
    {
        event_dispatcher dispatcher(e);

        // Handle window resize
        dispatcher.dispatch<window_resize_event>([this](window_resize_event& resize_event) {
            on_resize(resize_event.get_width(), resize_event.get_height());
            return false;
        });

        for (auto it = m_layer_stack.rbegin(); it != m_layer_stack.rend(); ++it)
        {
            if (e.is_handled())
                break;
            (*it)->on_event(e);
        }
    }

    void application::on_resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;

        m_device->resize(width, height);
    }

    void application::post_event(event& e)
    {
        on_event(e);
    }

    void application::queue_event(std::unique_ptr<event> e)
    {
        m_event_queue.push_back(std::move(e));
    }

    void application::run()
    {
        m_window->show();

        float last_time = get_time();

        while (m_running)
        {
            m_window->update();

            if (m_window->should_close())
            {
                shutdown();
                break;
            }

            // Process queued events
            for (auto& e : m_event_queue)
                on_event(*e);
            m_event_queue.clear();

            float current_time = get_time();
            float timestep = std::clamp(current_time - last_time, 0.001f, 0.1f);
            last_time = current_time;

            // Update layers
            for (auto& layer : m_layer_stack)
                layer->on_update(timestep);

            begin_frame();

            if (m_specs.enable_imgui)
            {
                // Setup dockspace
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);

                ImGuiWindowFlags window_flags =
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoBringToFrontOnFocus |
                    ImGuiWindowFlags_NoNavFocus |
                    ImGuiWindowFlags_NoBackground;

                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

                ImGui::Begin("lumina_dockspace", nullptr, window_flags);
                ImGui::PopStyleVar(3);

                ImGuiID dockspace_id = ImGui::GetID("lumina_dockspace_id");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
            }

            // Render layers
            for (auto& layer : m_layer_stack)
                layer->on_render();

            if (m_specs.enable_imgui)
            {
                ImGui::End();
            }

            end_frame();
        }
    }

    void application::set_fullscreen(bool fullscreen)
    {
        m_window->set_fullscreen(fullscreen);
    }

    void application::set_vsync(bool enabled)
    {
        m_window->set_vsync(enabled);
    }

    void application::set_position(int32_t x, int32_t y)
    {
        m_window->set_position(x, y);
    }

    void application::maximize()
    {
        m_window->maximize();
    }

    void application::minimize()
    {
        m_window->minimize();
    }

    void application::restore()
    {
        m_window->restore();
    }

    uint32_t application::get_width() const
    {
        return m_window->get_width();
    }

    uint32_t application::get_height() const
    {
        return m_window->get_height();
    }

    bool application::is_fullscreen() const
    {
        return m_window->is_fullscreen();
    }

    bool application::is_vsync() const
    {
        return m_window->is_vsync();
    }

    bool application::is_maximized() const
    {
        return m_window->is_maximized();
    }

    bool application::is_minimized() const
    {
        return m_window->is_minimized();
    }

    float application::get_time()
    {
        return static_cast<float>(glfwGetTime());
    }
}
