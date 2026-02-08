// Lumina UI Demo Window
// Demonstrates all ui:: functions for testing and style development

#include "demo.h"
#include "ui.h"

#include <glm/glm.hpp>

namespace lumina::ui
{
    // Demo state
    static struct
    {
        // Text demo
        bool show_text = true;

        // Buttons demo
        bool show_buttons = true;
        bool toggle_state = false;

        // Properties demo
        bool show_properties = true;
        float prop_float = 1.5f;
        int prop_int = 42;
        bool prop_bool = true;
        char prop_string[128] = "Hello Lumina";
        std::string prop_std_string = "std::string value";
        glm::vec2 prop_vec2 = {1.0f, 2.0f};
        glm::vec3 prop_vec3 = {1.0f, 2.0f, 3.0f};
        glm::vec4 prop_vec4 = {1.0f, 2.0f, 3.0f, 4.0f};
        glm::vec3 prop_color3 = {1.0f, 0.5f, 0.0f};
        glm::vec4 prop_color4 = {1.0f, 0.5f, 0.0f, 1.0f};
        float prop_slider = 0.5f;
        int prop_slider_int = 50;
        float prop_angle = 0.0f;
        int prop_dropdown = 0;

        // Panels demo
        bool show_panels = true;
        bool section_visible = true;

        // Layouts demo
        bool show_layouts = true;

        // Tables demo
        bool show_tables = true;

        // Trees demo
        bool show_trees = true;
        int selected_tree_item = -1;

        // Menus demo
        bool show_menus = true;
        bool menu_toggle = false;

        // Toolbars demo
        bool show_toolbars = true;
        bool toolbar_toggle1 = false;
        bool toolbar_toggle2 = true;

        // Popups demo
        bool show_popups = true;
        char input_buffer[128] = "";

        // Tabs demo
        bool show_tabs = true;

        // State queries demo
        bool show_state = true;

        // Icons demo
        bool show_icons = true;

        // Notifications demo
        bool show_notifications = true;
    } s_demo;

    static void demo_help_marker(const char* desc)
    {
        text_disabled("(?)");
        if (is_item_hovered(ImGuiHoveredFlags_DelayShort))
        {
            if (begin_tooltip())
            {
                push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
                text_wrapped(desc);
                pop_style_var();
                end_tooltip();
            }
        }
    }

    static void show_text_demo()
    {
        if (begin_section("Text", ImGuiTreeNodeFlags_DefaultOpen))
        {
            text("text() - Basic text");
            text(std::string("text(std::string) - String overload"));

            text_colored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "text_colored() - Red text");
            text_colored(glm::vec4(0.4f, 1.0f, 0.4f, 1.0f), "text_colored(glm::vec4) - Green text");
            text_colored(glm::vec3(0.4f, 0.4f, 1.0f), "text_colored(glm::vec3) - Blue text");

            text_disabled("text_disabled() - Grayed out text");
            text_wrapped("text_wrapped() - This is a longer piece of text that will wrap to multiple lines when the window is too narrow to contain it all on one line.");

            text_bullet("text_bullet() - Bulleted item 1");
            text_bullet("text_bullet() - Bulleted item 2");

            text_fmt("text_fmt() - Formatted: {} + {} = {}", 1, 2, 3);
            text_colored_fmt({1.0f, 1.0f, 0.4f, 1.0f}, "text_colored_fmt() - Answer: {}", 42);

            text_label("text_label()", "label: value");

            end_section();
        }
    }

    static void show_buttons_demo()
    {
        if (begin_section("Buttons & Basic Widgets"))
        {
            if (button("button()"))
                notify("Button clicked!", notification_type::info);

            same_line();
            if (button_small("button_small()"))
                notify("Small button clicked!", notification_type::info);

            if (button_primary("button_primary()"))
                notify("Primary action!", notification_type::success);

            same_line();
            if (button_success("button_success()"))
                notify("Success!", notification_type::success);

            same_line();
            if (button_danger("button_danger()"))
                notify("Danger!", notification_type::error);

            separator();

            checkbox("checkbox()", s_demo.toggle_state);
            same_line();
            text_fmt("State: {}", s_demo.toggle_state);

            separator();

            text("selectable():");
            static int selected = 0;
            for (int i = 0; i < 3; i++)
            {
                push_id(i);
                if (selectable("Selectable item", selected == i))
                    selected = i;
                pop_id();
            }

            end_section();
        }
    }

    static void show_properties_demo()
    {
        if (begin_section("Properties"))
        {
            text("Property editor with label-left, widget-right layout:");
            separator();

            property("Float", s_demo.prop_float);
            property("Int", s_demo.prop_int);
            property("Bool", s_demo.prop_bool);
            property("char[]", s_demo.prop_string, sizeof(s_demo.prop_string));
            property("std::string", s_demo.prop_std_string);

            separator();
            text("Vectors (glm):");

            property("Vec2", s_demo.prop_vec2);
            property("Vec3", s_demo.prop_vec3);
            property("Vec4", s_demo.prop_vec4);

            separator();
            text("Colors:");

            property_color("Color RGB", s_demo.prop_color3);
            property_color("Color RGBA", s_demo.prop_color4);

            separator();
            text("Sliders:");

            property_slider("Slider Float", s_demo.prop_slider, 0.0f, 1.0f);
            property_slider("Slider Int", s_demo.prop_slider_int, 0, 100);
            property_angle("Angle", s_demo.prop_angle);

            separator();
            text("Dropdown:");

            static const char* items[] = {"Option A", "Option B", "Option C"};
            property_dropdown("Dropdown", s_demo.prop_dropdown, items, 3);

            separator();
            text("Read-only:");

            property_text("Text Property", "Read-only value");

            separator();
            text("Custom widget:");

            property_custom("Custom", []() {
                return ImGui::Button("Click Me!");
            });

            end_section();
        }
    }

    static void show_panels_demo()
    {
        if (begin_section("Panels & Sections"))
        {
            text("begin_panel() / end_panel() - Styled window with padding");
            text("begin_section() / end_section() - Collapsing sections");

            spacing();

            if (begin_section("Nested Section 1", s_demo.section_visible))
            {
                text("Content inside section 1");
                text("Sections auto-indent content");
                end_section();
            }

            if (begin_section("Nested Section 2"))
            {
                text("Content inside section 2");
                if (begin_section("Deeply Nested"))
                {
                    text("Sections can be nested");
                    end_section();
                }
                end_section();
            }

            end_section();
        }
    }

    static void show_layouts_demo()
    {
        if (begin_section("Layouts"))
        {
            text("separator():");
            separator();

            text("spacing():");
            spacing();
            spacing();
            spacing();
            text("(3x spacing above)");

            separator();

            text("same_line():");
            button("Button 1"); same_line();
            button("Button 2"); same_line();
            button("Button 3");

            separator();

            text("indent() / unindent():");
            indent();
            text("Indented once");
            indent();
            text("Indented twice");
            unindent();
            text("Back to one indent");
            unindent();
            text("No indent");

            separator();

            text("dummy() - invisible spacer:");
            button("Before");
            dummy(ImVec2(50, 20));
            same_line();
            text("<- 50x20 dummy");
            button("After");

            separator();

            text("push_item_width() / set_next_item_width():");
            set_next_item_width(100);
            static float w1 = 0;
            ImGui::DragFloat("##w1", &w1);
            same_line(); text("<- 100px width");

            push_item_width(200);
            static float w2 = 0;
            ImGui::DragFloat("##w2", &w2);
            same_line(); text("<- 200px width");
            pop_item_width();

            separator();

            text("push_id() / pop_id():");
            for (int i = 0; i < 3; i++)
            {
                push_id(i);
                button("Same Label");
                same_line();
                pop_id();
            }
            new_line();
            text("(3 buttons with same label, different IDs)");

            separator();

            text_fmt("get_content_width(): {:.0f}", get_content_width());
            text_fmt("get_content_height(): {:.0f}", get_content_height());

            end_section();
        }
    }

    static void show_tables_demo()
    {
        if (begin_section("Tables"))
        {
            if (begin_table("demo_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                table_setup_column("ID");
                table_setup_column("Name");
                table_setup_column("Value");
                table_headers_row();

                for (int i = 0; i < 5; i++)
                {
                    table_next_row();
                    table_next_column(); text_fmt("{}", i);
                    table_next_column(); text_fmt("Item {}", i);
                    table_next_column(); text_fmt("{:.2f}", i * 1.5f);
                }

                end_table();
            }

            end_section();
        }
    }

    static void show_trees_demo()
    {
        if (begin_section("Trees"))
        {
            text("tree_node_leaf() - leaf nodes with selection:");

            for (int i = 0; i < 3; i++)
            {
                push_id(i);
                if (tree_node_leaf(i == 0 ? "Leaf Node A" : (i == 1 ? "Leaf Node B" : "Leaf Node C"),
                                   s_demo.selected_tree_item == i))
                {
                    s_demo.selected_tree_item = i;
                }
                pop_id();
            }

            separator();

            text("tree_node_selectable() - expandable with selection:");

            if (begin_tree_node("Parent Node", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (tree_node_leaf("Child 1", s_demo.selected_tree_item == 10))
                    s_demo.selected_tree_item = 10;
                if (tree_node_leaf("Child 2", s_demo.selected_tree_item == 11))
                    s_demo.selected_tree_item = 11;

                if (begin_tree_node("Nested Parent"))
                {
                    if (tree_node_leaf("Grandchild", s_demo.selected_tree_item == 20))
                        s_demo.selected_tree_item = 20;
                    end_tree_node();
                }
                end_tree_node();
            }

            end_section();
        }
    }

    static void show_menus_demo()
    {
        if (begin_section("Menus"))
        {
            text("Right-click here for context_menu_window():");

            context_menu_window([&]() {
                if (menu_item("Action 1"))
                    notify("Action 1 selected");
                if (menu_item("Action 2"))
                    notify("Action 2 selected");
                menu_separator();
                menu_item("Toggle Option", s_demo.menu_toggle);
            });

            separator();

            button("Right-click me");
            context_menu([&]() {
                if (menu_item("Item Context Action"))
                    notify("Item action!");
            });

            separator();

            text("Menu bar in window (use ImGuiWindowFlags_MenuBar):");
            text("begin_menu_bar() / end_menu_bar()");
            text("begin_menu() / end_menu()");
            text("menu_item() / menu_separator()");

            end_section();
        }
    }

    static void show_toolbars_demo()
    {
        if (begin_section("Toolbars"))
        {
            text("toolbar() with toolbar_button() and toolbar_toggle():");

            toolbar([&]() {
                if (toolbar_button("New"))
                    notify("New clicked");
                if (toolbar_button("Open"))
                    notify("Open clicked");
                if (toolbar_button("Save"))
                    notify("Save clicked");

                toolbar_separator();

                toolbar_toggle("Toggle1", s_demo.toolbar_toggle1);
                toolbar_toggle("Toggle2", s_demo.toolbar_toggle2);
            });

            text_fmt("Toggle1: {}, Toggle2: {}", s_demo.toolbar_toggle1, s_demo.toolbar_toggle2);

            end_section();
        }
    }

    static void show_popups_demo()
    {
        if (begin_section("Popups & Dialogs"))
        {
            if (button("Open Popup"))
                open_popup("demo_popup");

            if (begin_popup("demo_popup"))
            {
                text("This is a popup!");
                if (button("Close"))
                    close_current_popup();
                end_popup();
            }

            separator();

            if (button("Open Modal"))
                open_popup("Demo Modal");

            if (begin_popup_modal("Demo Modal", ImGuiWindowFlags_AlwaysAutoResize))
            {
                text("This is a modal dialog.");
                text("Click outside won't close it.");
                separator();
                if (button("OK", {120, 0}))
                    close_current_popup();
                same_line();
                if (button("Cancel", {120, 0}))
                    close_current_popup();
                end_popup();
            }

            separator();

            if (button("Confirm Dialog"))
                open_popup("Confirm?");

            confirm_dialog("Confirm?", "Are you sure you want to proceed?",
                []() { notify("Confirmed!", notification_type::success); },
                []() { notify("Cancelled", notification_type::warning); }
            );

            separator();

            if (button("Input Dialog"))
                open_popup("Enter Name");

            input_dialog("Enter Name", "Please enter your name:",
                s_demo.input_buffer, sizeof(s_demo.input_buffer),
                [&]() { notify(std::string("Hello, ") + s_demo.input_buffer, notification_type::info); }
            );

            end_section();
        }
    }

    static void show_tabs_demo()
    {
        if (begin_section("Tabs"))
        {
            if (begin_tab_bar("demo_tabs"))
            {
                if (begin_tab_item("Tab 1"))
                {
                    text("Content of Tab 1");
                    end_tab_item();
                }
                if (begin_tab_item("Tab 2"))
                {
                    text("Content of Tab 2");
                    end_tab_item();
                }
                if (begin_tab_item("Tab 3"))
                {
                    text("Content of Tab 3");
                    end_tab_item();
                }
                end_tab_bar();
            }

            end_section();
        }
    }

    static void show_state_demo()
    {
        if (begin_section("State Queries"))
        {
            button("Hover or Click Me");

            text_fmt("is_item_hovered(): {}", is_item_hovered());
            text_fmt("is_item_active(): {}", is_item_active());
            text_fmt("is_item_clicked(): {}", is_item_clicked());

            separator();

            text_fmt("is_window_focused(): {}", is_window_focused());
            text_fmt("is_window_hovered(): {}", is_window_hovered());

            separator();

            auto pos = get_cursor_pos();
            text_fmt("get_cursor_pos(): ({:.0f}, {:.0f})", pos.x, pos.y);

            auto screen_pos = get_cursor_screen_pos();
            text_fmt("get_cursor_screen_pos(): ({:.0f}, {:.0f})", screen_pos.x, screen_pos.y);

            separator();

            auto mouse = get_mouse_pos();
            text_fmt("get_mouse_pos(): ({:.0f}, {:.0f})", mouse.x, mouse.y);
            text_fmt("is_mouse_down(Left): {}", is_mouse_down(ImGuiMouseButton_Left));

            end_section();
        }
    }

    static void show_icons_demo()
    {
        if (begin_section("Icons"))
        {
            text("Placeholder icons (replace with font icons later):");
            spacing();

            icon_button(icons::folder, "Folder");
            same_line();
            icon_button(icons::file, "File");
            same_line();
            icon_button(icons::save, "Save");
            same_line();
            icon_button(icons::open, "Open");

            spacing();

            icon_button(icons::play);
            same_line();
            icon_button(icons::pause);
            same_line();
            icon_button(icons::stop);
            same_line();
            icon_button(icons::add);
            same_line();
            icon_button(icons::remove);

            end_section();
        }
    }

    static void show_notifications_demo()
    {
        if (begin_section("Notifications"))
        {
            text("notify() - Toast notifications:");
            spacing();

            if (button("Info"))
                notify("This is an info notification", notification_type::info);
            same_line();
            if (button("Success"))
                notify("Operation successful!", notification_type::success);
            same_line();
            if (button("Warning"))
                notify("Warning: Check your input", notification_type::warning);
            same_line();
            if (button("Error"))
                notify("Error: Something went wrong", notification_type::error);

            spacing();
            text("Call render_notifications() at end of frame to display.");

            end_section();
        }
    }

    static void show_style_demo()
    {
        if (begin_section("Style"))
        {
            text("push_style_color() / pop_style_color():");
            push_style_color(ImGuiCol_Text, ImVec4(1, 0.5f, 0, 1));
            text("Orange text");
            pop_style_color();

            spacing();

            text("push_style_var() / pop_style_var():");
            push_style_var(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
            button("Extra Padding");
            pop_style_var();

            same_line();
            button("Normal Padding");

            end_section();
        }
    }

    static void show_child_windows_demo()
    {
        if (begin_section("Child Windows"))
        {
            text("begin_child() / end_child():");

            // Note: end_child() must ALWAYS be called after begin_child(), unlike begin_window()
            begin_child("child1", {0, 100}, ImGuiChildFlags_Borders);
            for (int i = 0; i < 20; i++)
                text_fmt("Scrollable item {}", i);
            end_child();

            spacing();

            text("begin_group() / end_group() for layout:");
            begin_group();
            button("Grouped");
            button("Buttons");
            end_group();

            same_line();

            begin_group();
            text("Grouped");
            text("Text");
            end_group();

            end_section();
        }
    }

    static void show_disabled_demo()
    {
        if (begin_section("Disabled State"))
        {
            text("begin_disabled() / end_disabled():");

            begin_disabled(true);
            button("Disabled Button");
            static float val = 0.5f;
            ImGui::SliderFloat("Disabled Slider", &val, 0, 1);
            end_disabled();

            button("Enabled Button");

            end_section();
        }
    }

    static void show_combos_listboxes_demo()
    {
        if (begin_section("Combos & Listboxes"))
        {
            text("begin_combo() / end_combo():");

            static int combo_item = 0;
            static const char* items[] = {"Apple", "Banana", "Cherry", "Date"};

            if (begin_combo("Fruits", items[combo_item]))
            {
                for (int i = 0; i < 4; i++)
                {
                    bool selected = (combo_item == i);
                    if (selectable(items[i], static_cast<bool>(selected), ImGuiSelectableFlags_None))
                        combo_item = i;
                    if (selected)
                        set_item_default_focus();
                }
                end_combo();
            }

            separator();

            text("begin_listbox() / end_listbox():");

            static int listbox_item = 0;
            if (begin_listbox("##listbox", {-1, 80}))
            {
                for (int i = 0; i < 4; i++)
                {
                    if (selectable(items[i], listbox_item == i))
                        listbox_item = i;
                }
                end_listbox();
            }

            end_section();
        }
    }

    void show_demo_window(bool* p_open)
    {
        if (p_open && !*p_open)
            return;

        ImGui::SetNextWindowSize(ImVec2(600, 800), ImGuiCond_FirstUseEver);

        bool is_open = true;
        bool& open_ref = p_open ? *p_open : is_open;
        if (!begin_window("Lumina UI Demo", open_ref, ImGuiWindowFlags_None))
        {
            end_window();
            return;
        }

        text("Lumina UI Module Demo");
        text_disabled("All functions use the ui:: namespace");
        separator();

        show_text_demo();
        show_buttons_demo();
        show_properties_demo();
        show_panels_demo();
        show_layouts_demo();
        show_style_demo();
        show_child_windows_demo();
        show_disabled_demo();
        show_tables_demo();
        show_trees_demo();
        show_menus_demo();
        show_toolbars_demo();
        show_popups_demo();
        show_tabs_demo();
        show_combos_listboxes_demo();
        show_state_demo();
        show_icons_demo();
        show_notifications_demo();

        end_window();
    }
}
