#include <lumina/core/core.h>
#include <lumina/core/entry_point.h>
#include <lumina/ui/ui.h>

#include <glm/glm.hpp>

class sandbox_layer : public lumina::core::layer
{
public:
    sandbox_layer() : layer("sandbox") {}

    void on_attach() override
    {
        LUMINA_LOG_INFO("Sandbox layer attached");

        m_dock_layout.split("Inspector", lumina::ui::dock_position::right, 0.25f);
        m_dock_layout.split("Scene Hierarchy", lumina::ui::dock_position::left, 0.20f);
        m_dock_layout.split("Viewport", lumina::ui::dock_position::center);
        m_dock_layout.sub_split("Console", "Viewport", lumina::ui::dock_position::bottom, 0.25f);
        m_dock_layout.tabbed("Statistics", "Console");
    }

    void on_detach() override
    {
        LUMINA_LOG_INFO("Sandbox layer detached");
    }

    void on_update(float dt) override
    {
        m_frame_time = dt;
    }

    void on_render() override
    {
        namespace ui = lumina::ui;

        // Apply docking layout on first frame
        if (m_dock_layout.has_requests())
        {
            ImGuiID dockspace_id = ui::get_id("lumina_dockspace_id");
            m_dock_layout.apply(dockspace_id);
        }

        // --- Inspector panel ---
        ui::panel("Inspector", [&]
        {
            ui::section("Transform", [&]
            {
                ui::property("Position", m_position);
                ui::property("Rotation", m_rotation);
                ui::property("Scale", m_scale, 0.01f);
            });

            ui::section("Material", [&]
            {
                ui::property_color("Albedo", m_albedo);
                ui::property_slider("Roughness", m_roughness, 0.0f, 1.0f);
                ui::property_slider("Metallic", m_metallic, 0.0f, 1.0f);
                ui::property("Emission", m_emission, 0.01f, 0.0f, 10.0f);
                ui::property_color("Emission Color", m_emission_color);
            });

            ui::section("Rendering", [&]
            {
                ui::property("Wireframe", m_wireframe);
                ui::property("Cast Shadows", m_cast_shadows);

                static const char* modes[] = { "Opaque", "Transparent", "Cutout" };
                ui::property_dropdown("Blend Mode", m_blend_mode, std::span(modes));
            });

            ui::separator();

            if (ui::button_primary("Apply", ImVec2(-1, 0)))
                ui::notify("Settings applied!", ui::notification_type::success);

            if (ui::button_danger("Reset All", ImVec2(-1, 0)))
            {
                m_position = glm::vec3(0.0f);
                m_rotation = glm::vec3(0.0f);
                m_scale = glm::vec3(1.0f);
                m_albedo = glm::vec4(1.0f);
                m_roughness = 0.5f;
                m_metallic = 0.0f;
                m_emission = 0.0f;
                m_emission_color = glm::vec3(1.0f);
                m_wireframe = false;
                m_cast_shadows = true;
                m_blend_mode = 0;
                ui::notify("All properties reset", ui::notification_type::info);
            }
        });

        // --- Scene hierarchy panel ---
        ui::panel("Scene Hierarchy", [&]
        {
            ui::tree_node("Root", [&]
            {
                if (ui::tree_node_leaf("Camera", m_selected == 0))
                    m_selected = 0;

                ui::tree_node_selectable("Cube", m_selected == 1, [&]
                {
                    if (ui::tree_node_leaf("Mesh", false))
                        {}
                    if (ui::tree_node_leaf("Material", false))
                        {}
                });
                if (ui::is_item_clicked() && !ui::is_item_toggled_open())
                    m_selected = 1;

                if (ui::tree_node_leaf("Directional Light", m_selected == 2))
                    m_selected = 2;

                if (ui::tree_node_leaf("Point Light", m_selected == 3))
                    m_selected = 3;

            }, ImGuiTreeNodeFlags_DefaultOpen);

            ui::separator();

            ui::context_menu_window([&]
            {
                if (ui::menu_item("Add Empty Entity"))
                    ui::notify("Entity created", ui::notification_type::success);
                if (ui::menu_item("Add Cube"))
                    ui::notify("Cube created", ui::notification_type::success);
                if (ui::menu_item("Add Light"))
                    ui::notify("Light created", ui::notification_type::success);
                ui::menu_separator();
                if (ui::menu_item("Delete Selected"))
                    ui::notify("Entity deleted", ui::notification_type::warning);
            });
        });

        // --- Viewport panel ---
        ui::panel("Viewport", [&]
        {
            ui::toolbar([&]
            {
                if (ui::toolbar_button("Play"))
                    ui::notify("Play mode started", ui::notification_type::info);
                if (ui::toolbar_button("Pause"))
                    ui::notify("Paused", ui::notification_type::warning);
                if (ui::toolbar_button("Stop"))
                    ui::notify("Stopped", ui::notification_type::error);

                ui::toolbar_separator();

                ui::toolbar_toggle("Grid", m_show_grid);
                ui::toolbar_toggle("Gizmo", m_show_gizmo);
            });

            ui::text("Viewport placeholder");
            ui::text_disabled("(Renderer not yet connected)");
            ui::spacing();

            glm::vec4 info_color(0.6f, 0.8f, 1.0f, 1.0f);
            ui::text_colored(info_color, "Camera:");
            ui::same_line();
            ui::text_fmt("Perspective ({:.0f} FOV)", 60.0f);
        });

        // --- Console panel ---
        ui::panel("Console", [&]
        {
            ui::main_menu_bar([&]
            {
                // This won't render here since we're inside a panel,
                // but demonstrates the API pattern
            });

            static bool show_info = true, show_warn = true, show_error = true;
            ui::toolbar([&]
            {
                ui::toolbar_toggle("Info", show_info);
                ui::toolbar_toggle("Warn", show_warn);
                ui::toolbar_toggle("Error", show_error);
                ui::toolbar_separator();
                if (ui::toolbar_button("Clear"))
                    {}
            });

            // Sample log entries with colored text
            if (show_info)
            {
                ui::text_colored(lumina::core::color::text_secondary, "[INFO] Application initialized");
                ui::text_colored(lumina::core::color::text_secondary, "[INFO] Vulkan device created");
            }
            if (show_warn)
                ui::text_colored(lumina::core::color::warning, "[WARN] Shader compilation took 250ms");
            if (show_error)
                ui::text_colored(lumina::core::color::error, "[ERROR] Missing texture: default.png");
        });

        // --- Statistics panel (tabbed with Console) ---
        ui::panel("Statistics", [&]
        {
            ui::text_fmt("Frame Time: {:.3f} ms", m_frame_time * 1000.0f);
            ui::text_fmt("FPS: {:.1f}", m_frame_time > 0.0f ? 1.0f / m_frame_time : 0.0f);
            ui::separator();

            ui::table("##stats", 2, [&]
            {
                ui::table_setup_column("Metric", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ui::table_setup_column("Value");
                ui::table_headers_row();

                ui::table_next_row();
                ui::table_next_column(); ui::text("Draw Calls");
                ui::table_next_column(); ui::text("42");

                ui::table_next_row();
                ui::table_next_column(); ui::text("Triangles");
                ui::table_next_column(); ui::text("128,456");

                ui::table_next_row();
                ui::table_next_column(); ui::text("Vertices");
                ui::table_next_column(); ui::text("384,200");

                ui::table_next_row();
                ui::table_next_column(); ui::text("Textures Bound");
                ui::table_next_column(); ui::text("12");

            }, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable);
        });

        // --- Tab bar demo (standalone window) ---
        ui::window("Tabs Demo", [&]
        {
            ui::tab_bar("##demo_tabs", [&]
            {
                ui::tab_item("Properties", [&]
                {
                    ui::property_text("Type", "StaticMesh");
                    ui::property_text("Vertices", "1024");
                    ui::property("Visible", m_entity_visible);
                    ui::property("Name", m_entity_name);
                });

                ui::tab_item("Events", [&]
                {
                    ui::text_bullet("OnCollisionEnter");
                    ui::text_bullet("OnTriggerExit");
                    ui::text_bullet("OnUpdate");
                });

                ui::tab_item("Debug", [&]
                {
                    ui::text_wrapped("This tab demonstrates text wrapping. "
                        "Long text content will automatically wrap to fit "
                        "the available width of the tab content area.");
                    ui::spacing();
                    ui::property_angle("FOV", m_fov);
                });
            });
        });

        // --- Dialog demo ---
        ui::window("Dialogs Demo", [&]
        {
            if (ui::button("Show Confirm Dialog"))
                ui::open_popup("Confirm Delete");

            if (ui::button("Show Input Dialog"))
                ui::open_popup("Rename Entity");

            ui::confirm_dialog("Confirm Delete",
                "Are you sure you want to delete this entity?\nThis action cannot be undone.",
                [&] { ui::notify("Entity deleted!", ui::notification_type::warning); },
                [&] { ui::notify("Cancelled", ui::notification_type::info); });

            static char rename_buf[128] = "Entity_01";
            ui::input_dialog("Rename Entity", "Enter new name:", rename_buf, sizeof(rename_buf),
                [&] { ui::notify(std::string("Renamed to: ") + rename_buf, ui::notification_type::success); });
        });

        // Render notifications overlay (always last)
        ui::render_notifications();
    }

    void on_event(lumina::core::event& e) override
    {
        lumina::core::event_dispatcher dispatcher(e);

        dispatcher.dispatch<lumina::core::key_pressed_event>([](lumina::core::key_pressed_event& e)
        {
            if (e.get_key() == lumina::core::input::key_code::escape)
            {
                lumina::core::application::get().shutdown();
                return true;
            }
            return false;
        });
    }

private:
    lumina::ui::dock_layout m_dock_layout;

    // Transform
    glm::vec3 m_position{0.0f, 0.0f, 0.0f};
    glm::vec3 m_rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 m_scale{1.0f, 1.0f, 1.0f};

    // Material
    glm::vec4 m_albedo{1.0f, 1.0f, 1.0f, 1.0f};
    float m_roughness = 0.5f;
    float m_metallic = 0.0f;
    float m_emission = 0.0f;
    glm::vec3 m_emission_color{1.0f, 1.0f, 1.0f};

    // Rendering
    bool m_wireframe = false;
    bool m_cast_shadows = true;
    int m_blend_mode = 0;

    // Viewport
    bool m_show_grid = true;
    bool m_show_gizmo = true;

    // Scene
    int m_selected = -1;

    // Entity properties
    bool m_entity_visible = true;
    std::string m_entity_name = "Cube_01";
    float m_fov = 1.0472f; // 60 degrees

    // Stats
    float m_frame_time = 0.0f;
};

lumina::core::application* lumina::core::create_application(int argc, char** argv)
{
    lumina::core::application_spec spec;
    spec.name = "Lumina UI Sandbox";
    spec.width = 1600;
    spec.height = 900;
    // spec.api = lumina::core::graphics_api::d3d12;

    auto* app = new lumina::core::application(spec);
    app->push_layer<sandbox_layer>();

    return app;
}
