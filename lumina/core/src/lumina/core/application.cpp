#include "application.h"
#include "log.h"
#include "assert.h"
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

    application::application(const application_spec& spec)
        : m_spec(spec)
    {
        LUMINA_ASSERT(!s_instance, "Application already exists");
        s_instance = this;

        log::init(m_spec.name);
        LUMINA_LOG_INFO("Starting Lumina Application: {}", m_spec.name);

        // Create window
        window_spec win_spec;
        win_spec.title = m_spec.name;
        win_spec.icon_path = m_spec.icon;
        win_spec.width = m_spec.width;
        win_spec.height = m_spec.height;
        win_spec.position_x = m_spec.position_x;
        win_spec.position_y = m_spec.position_y;
        win_spec.fullscreen = m_spec.fullscreen;
        win_spec.maximized = m_spec.maximized;

        m_window = make_scope<window>(win_spec);
        m_window->set_event_callback([this](event& e) { on_event(e); });
        LUMINA_ASSERT(m_window, "Failed to create application window");

        // Initialize graphics device
        m_graphics_device = graphics_device::create(m_spec.api);
        LUMINA_ASSERT(m_graphics_device, "Failed to create graphics device");

        graphics_device_desc gfx_desc;
        gfx_desc.window = m_window->get_native_window();
        gfx_desc.width = m_window->get_width();
        gfx_desc.height = m_window->get_height();
        gfx_desc.vsync = m_spec.vsync;
        gfx_desc.app_name = m_spec.name;
#ifdef LUMINA_DEBUG
        gfx_desc.enable_debug_layer = true;
#endif

        bool gfx_init = m_graphics_device->init(gfx_desc);
        LUMINA_ASSERT(gfx_init, "Failed to initialize graphics device");

        init_imgui();
    }

    application::~application()
    {
        m_event_queue.clear();

        for (auto& layer : m_layer_stack)
            layer->on_detach();

        m_layer_stack.clear();

        shutdown_imgui();

        m_graphics_device->shutdown();
        m_graphics_device.reset();

        m_window.reset();

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
        // TODO: Enable viewports - requires per-window NVRHI swapchain/device and platform rendering callbacks
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

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
        imgui_config.device = m_graphics_device->get_device();
        imgui_config.render_target_format = m_graphics_device->get_swapchain_format();

        bool imgui_init = imgui::init(imgui_config);
        LUMINA_ASSERT(imgui_init, "Failed to initialize ImGui NVRHI backend");

        if (m_spec.dark_theme)
            ImGui::StyleColorsDark();
        else
            ImGui::StyleColorsLight();

        LUMINA_LOG_INFO("ImGui initialized");
    }

    void application::shutdown_imgui()
    {
        imgui::shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void application::begin_frame()
    {
        m_graphics_device->begin_frame();

        // Clear the framebuffer
        auto cmd = m_graphics_device->get_command_list();
        auto fb = m_graphics_device->get_current_framebuffer();

        nvrhi::utils::ClearColorAttachment(cmd, fb, 0, nvrhi::Color(0.1f, 0.1f, 0.1f, 1.0f));

        imgui::new_frame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void application::end_frame()
    {
        ImGui::Render();

        // Render ImGui draw data using NVRHI
        auto cmd = m_graphics_device->get_command_list();
        auto fb = m_graphics_device->get_current_framebuffer();
        imgui::render_draw_data(cmd, fb, ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        m_graphics_device->present();
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

        m_graphics_device->resize(width, height);
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

            // Setup dockspace if enabled
            if (m_spec.enable_docking)
            {
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

                ImGui::Begin("lumian_dockspace", nullptr, window_flags);
                ImGui::PopStyleVar(3);

                ImGuiID dockspace_id = ImGui::GetID("lumina_dockspace_id");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
            }

            // Render layers
            for (auto& layer : m_layer_stack)
                layer->on_render();

            if (m_spec.enable_docking)
            {
                ImGui::End();
            }

            end_frame();
        }
    }

    void application::set_fullscreen()
    {
        m_spec.fullscreen = !m_spec.fullscreen;
        m_window->set_fullscreen(m_spec.fullscreen);
    }

    float application::get_time()
    {
        return static_cast<float>(glfwGetTime());
    }
}
