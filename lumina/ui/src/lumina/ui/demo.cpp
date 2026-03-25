// Lumina UI Demo Window
// Demonstrates all ui:: functions for testing and style development

#include "demo.h"
#include "ui.h"

#include <glm/glm.hpp>

namespace Lumina::UI
{
    // Demo state
    static struct
    {
        // Text demo
        bool showText = true;

        // Buttons demo
        bool showButtons = true;
        bool toggleState = false;

        // Properties demo
        bool showProperties = true;
        float propFloat = 1.5f;
        int propInt = 42;
        bool propBool = true;
        char propString[128] = "Hello Lumina";
        std::string propStdString = "std::string value";
        glm::vec2 propVec2 = {1.0f, 2.0f};
        glm::vec3 propVec3 = {1.0f, 2.0f, 3.0f};
        glm::vec4 propVec4 = {1.0f, 2.0f, 3.0f, 4.0f};
        glm::vec3 propColor3 = {1.0f, 0.5f, 0.0f};
        glm::vec4 propColor4 = {1.0f, 0.5f, 0.0f, 1.0f};
        float propSlider = 0.5f;
        int propSliderInt = 50;
        float propAngle = 0.0f;
        int propDropdown = 0;

        // Panels demo
        bool showPanels = true;
        bool sectionVisible = true;

        // Layouts demo
        bool showLayouts = true;

        // Tables demo
        bool showTables = true;

        // Trees demo
        bool showTrees = true;
        int selectedTreeItem = -1;

        // Menus demo
        bool showMenus = true;
        bool menuToggle = false;

        // Toolbars demo
        bool showToolbars = true;
        bool toolbarToggle1 = false;
        bool toolbarToggle2 = true;

        // Popups demo
        bool showPopups = true;
        char inputBuffer[128] = "";

        // Tabs demo
        bool showTabs = true;

        // State queries demo
        bool showState = true;

        // Icons demo
        bool showIcons = true;

        // Notifications demo
        bool showNotifications = true;
    } s_Demo;

    static void DemoHelpMarker(const char* desc)
    {
        TextDisabled("(?)");
        if (IsItemHovered(ImGuiHoveredFlags_DelayShort))
        {
            if (BeginTooltip())
            {
                PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
                TextWrapped(desc);
                PopStyleVar();
                EndTooltip();
            }
        }
    }

    static void ShowTextDemo()
    {
        if (BeginSection("Text", ImGuiTreeNodeFlags_DefaultOpen))
        {
            Text("text() - Basic text");
            Text(std::string("text(std::string) - String overload"));

            TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "text_colored() - Red text");
            TextColored(glm::vec4(0.4f, 1.0f, 0.4f, 1.0f), "text_colored(glm::vec4) - Green text");
            TextColored(glm::vec3(0.4f, 0.4f, 1.0f), "text_colored(glm::vec3) - Blue text");

            TextDisabled("text_disabled() - Grayed out text");
            TextWrapped("text_wrapped() - This is a longer piece of text that will wrap to multiple lines when the window is too narrow to contain it all on one line.");

            TextBullet("text_bullet() - Bulleted item 1");
            TextBullet("text_bullet() - Bulleted item 2");

            TextFmt("text_fmt() - Formatted: {} + {} = {}", 1, 2, 3);
            TextColoredFmt({1.0f, 1.0f, 0.4f, 1.0f}, "text_colored_fmt() - Answer: {}", 42);

            TextLabel("text_label()", "label: value");

            EndSection();
        }
    }

    static void ShowButtonsDemo()
    {
        if (BeginSection("Buttons & Basic Widgets"))
        {
            if (Button("button()"))
                Notify("Button clicked!", NotificationType::Info);

            SameLine();
            if (ButtonSmall("button_small()"))
                Notify("Small button clicked!", NotificationType::Info);

            if (ButtonPrimary("button_primary()"))
                Notify("Primary action!", NotificationType::Success);

            SameLine();
            if (ButtonSuccess("button_success()"))
                Notify("Success!", NotificationType::Success);

            SameLine();
            if (ButtonDanger("button_danger()"))
                Notify("Danger!", NotificationType::Error);

            Separator();

            Checkbox("checkbox()", s_Demo.toggleState);
            SameLine();
            TextFmt("State: {}", s_Demo.toggleState);

            Separator();

            Text("selectable():");
            static int selected = 0;
            for (int i = 0; i < 3; i++)
            {
                PushID(i);
                if (Selectable("Selectable item", selected == i))
                    selected = i;
                PopID();
            }

            EndSection();
        }
    }

    static void ShowPropertiesDemo()
    {
        if (BeginSection("Properties"))
        {
            Text("Property editor with label-left, widget-right layout:");
            Separator();

            Property("Float", s_Demo.propFloat);
            Property("Int", s_Demo.propInt);
            Property("Bool", s_Demo.propBool);
            Property("char[]", s_Demo.propString, sizeof(s_Demo.propString));
            Property("std::string", s_Demo.propStdString);

            Separator();
            Text("Vectors (glm):");

            Property("Vec2", s_Demo.propVec2);
            Property("Vec3", s_Demo.propVec3);
            Property("Vec4", s_Demo.propVec4);

            Separator();
            Text("Colors:");

            PropertyColor("Color RGB", s_Demo.propColor3);
            PropertyColor("Color RGBA", s_Demo.propColor4);

            Separator();
            Text("Sliders:");

            PropertySlider("Slider Float", s_Demo.propSlider, 0.0f, 1.0f);
            PropertySlider("Slider Int", s_Demo.propSliderInt, 0, 100);
            PropertyAngle("Angle", s_Demo.propAngle);

            Separator();
            Text("Dropdown:");

            static const char* items[] = {"Option A", "Option B", "Option C"};
            PropertyDropdown("Dropdown", s_Demo.propDropdown, items, 3);

            Separator();
            Text("Read-only:");

            PropertyText("Text Property", "Read-only value");

            Separator();
            Text("Custom widget:");

            PropertyCustom("Custom", []() {
                return ImGui::Button("Click Me!");
            });

            EndSection();
        }
    }

    static void ShowPanelsDemo()
    {
        if (BeginSection("Panels & Sections"))
        {
            Text("begin_panel() / end_panel() - Styled window with padding");
            Text("begin_section() / end_section() - Collapsing sections");

            Spacing();

            if (BeginSection("Nested Section 1", s_Demo.sectionVisible))
            {
                Text("Content inside section 1");
                Text("Sections auto-indent content");
                EndSection();
            }

            if (BeginSection("Nested Section 2"))
            {
                Text("Content inside section 2");
                if (BeginSection("Deeply Nested"))
                {
                    Text("Sections can be nested");
                    EndSection();
                }
                EndSection();
            }

            EndSection();
        }
    }

    static void ShowLayoutsDemo()
    {
        if (BeginSection("Layouts"))
        {
            Text("separator():");
            Separator();

            Text("spacing():");
            Spacing();
            Spacing();
            Spacing();
            Text("(3x spacing above)");

            Separator();

            Text("same_line():");
            Button("Button 1"); SameLine();
            Button("Button 2"); SameLine();
            Button("Button 3");

            Separator();

            Text("indent() / unindent():");
            Indent();
            Text("Indented once");
            Indent();
            Text("Indented twice");
            Unindent();
            Text("Back to one indent");
            Unindent();
            Text("No indent");

            Separator();

            Text("dummy() - invisible spacer:");
            Button("Before");
            Dummy(50, 20);
            SameLine();
            Text("<- 50x20 dummy");
            Button("After");

            Separator();

            Text("push_item_width() / set_next_item_width():");
            SetNextItemWidth(100);
            static float w1 = 0;
            ImGui::DragFloat("##w1", &w1);
            SameLine(); Text("<- 100px width");

            PushItemWidth(200);
            static float w2 = 0;
            ImGui::DragFloat("##w2", &w2);
            SameLine(); Text("<- 200px width");
            PopItemWidth();

            Separator();

            Text("push_id() / pop_id():");
            for (int i = 0; i < 3; i++)
            {
                PushID(i);
                Button("Same Label");
                SameLine();
                PopID();
            }
            NewLine();
            Text("(3 buttons with same label, different IDs)");

            Separator();

            TextFmt("get_content_width(): {:.0f}", GetContentWidth());
            TextFmt("get_content_height(): {:.0f}", GetContentHeight());

            EndSection();
        }
    }

    static void ShowTablesDemo()
    {
        if (BeginSection("Tables"))
        {
            if (BeginTable("demo_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
            {
                TableSetupColumn("ID");
                TableSetupColumn("Name");
                TableSetupColumn("Value");
                TableHeadersRow();

                for (int i = 0; i < 5; i++)
                {
                    TableNextRow();
                    TableNextColumn(); TextFmt("{}", i);
                    TableNextColumn(); TextFmt("Item {}", i);
                    TableNextColumn(); TextFmt("{:.2f}", i * 1.5f);
                }

                EndTable();
            }

            EndSection();
        }
    }

    static void ShowTreesDemo()
    {
        if (BeginSection("Trees"))
        {
            Text("tree_node_leaf() - leaf nodes with selection:");

            for (int i = 0; i < 3; i++)
            {
                PushID(i);
                if (TreeNodeLeaf(i == 0 ? "Leaf Node A" : (i == 1 ? "Leaf Node B" : "Leaf Node C"),
                                   s_Demo.selectedTreeItem == i))
                {
                    s_Demo.selectedTreeItem = i;
                }
                PopID();
            }

            Separator();

            Text("tree_node_selectable() - expandable with selection:");

            if (BeginTreeNode("Parent Node", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (TreeNodeLeaf("Child 1", s_Demo.selectedTreeItem == 10))
                    s_Demo.selectedTreeItem = 10;
                if (TreeNodeLeaf("Child 2", s_Demo.selectedTreeItem == 11))
                    s_Demo.selectedTreeItem = 11;

                if (BeginTreeNode("Nested Parent"))
                {
                    if (TreeNodeLeaf("Grandchild", s_Demo.selectedTreeItem == 20))
                        s_Demo.selectedTreeItem = 20;
                    EndTreeNode();
                }
                EndTreeNode();
            }

            EndSection();
        }
    }

    static void ShowMenusDemo()
    {
        if (BeginSection("Menus"))
        {
            Text("Right-click here for context_menu_window():");

            ContextMenuWindow([&]() {
                if (MenuItem("Action 1"))
                    Notify("Action 1 selected");
                if (MenuItem("Action 2"))
                    Notify("Action 2 selected");
                MenuSeparator();
                MenuItem("Toggle Option", s_Demo.menuToggle);
            });

            Separator();

            Button("Right-click me");
            ContextMenu([&]() {
                if (MenuItem("Item Context Action"))
                    Notify("Item action!");
            });

            Separator();

            Text("Menu bar in window (use ImGuiWindowFlags_MenuBar):");
            Text("begin_menu_bar() / end_menu_bar()");
            Text("begin_menu() / end_menu()");
            Text("menu_item() / menu_separator()");

            EndSection();
        }
    }

    static void ShowToolbarsDemo()
    {
        if (BeginSection("Toolbars"))
        {
            Text("toolbar() with toolbar_button() and toolbar_toggle():");

            Toolbar([&]() {
                if (ToolbarButton("New"))
                    Notify("New clicked");
                if (ToolbarButton("Open"))
                    Notify("Open clicked");
                if (ToolbarButton("Save"))
                    Notify("Save clicked");

                ToolbarSeparator();

                ToolbarToggle("Toggle1", s_Demo.toolbarToggle1);
                ToolbarToggle("Toggle2", s_Demo.toolbarToggle2);
            });

            TextFmt("Toggle1: {}, Toggle2: {}", s_Demo.toolbarToggle1, s_Demo.toolbarToggle2);

            EndSection();
        }
    }

    static void ShowPopupsDemo()
    {
        if (BeginSection("Popups & Dialogs"))
        {
            if (Button("Open Popup"))
                OpenPopup("demo_popup");

            if (BeginPopup("demo_popup"))
            {
                Text("This is a popup!");
                if (Button("Close"))
                    CloseCurrentPopup();
                EndPopup();
            }

            Separator();

            if (Button("Open Modal"))
                OpenPopup("Demo Modal");

            if (BeginPopupModal("Demo Modal", ImGuiWindowFlags_AlwaysAutoResize))
            {
                Text("This is a modal dialog.");
                Text("Click outside won't close it.");
                Separator();
                if (Button("OK", {120, 0}))
                    CloseCurrentPopup();
                SameLine();
                if (Button("Cancel", {120, 0}))
                    CloseCurrentPopup();
                EndPopup();
            }

            Separator();

            if (Button("Confirm Dialog"))
                OpenPopup("Confirm?");

            ConfirmDialog("Confirm?", "Are you sure you want to proceed?",
                []() { Notify("Confirmed!", NotificationType::Success); },
                []() { Notify("Cancelled", NotificationType::Warning); }
            );

            Separator();

            if (Button("Input Dialog"))
                OpenPopup("Enter Name");

            InputDialog("Enter Name", "Please enter your name:",
                s_Demo.inputBuffer, sizeof(s_Demo.inputBuffer),
                [&]() { Notify(std::string("Hello, ") + s_Demo.inputBuffer, NotificationType::Info); }
            );

            EndSection();
        }
    }

    static void ShowTabsDemo()
    {
        if (BeginSection("Tabs"))
        {
            if (BeginTabBar("demo_tabs"))
            {
                if (BeginTabItem("Tab 1"))
                {
                    Text("Content of Tab 1");
                    EndTabItem();
                }
                if (BeginTabItem("Tab 2"))
                {
                    Text("Content of Tab 2");
                    EndTabItem();
                }
                if (BeginTabItem("Tab 3"))
                {
                    Text("Content of Tab 3");
                    EndTabItem();
                }
                EndTabBar();
            }

            EndSection();
        }
    }

    static void ShowStateDemo()
    {
        if (BeginSection("State Queries"))
        {
            Button("Hover or Click Me");

            TextFmt("is_item_hovered(): {}", IsItemHovered());
            TextFmt("is_item_active(): {}", IsItemActive());
            TextFmt("is_item_clicked(): {}", IsItemClicked());

            Separator();

            TextFmt("is_window_focused(): {}", IsWindowFocused());
            TextFmt("is_window_hovered(): {}", IsWindowHovered());

            Separator();

            auto pos = GetCursorPos();
            TextFmt("get_cursor_pos(): ({:.0f}, {:.0f})", pos.x, pos.y);

            auto screenPos = GetCursorScreenPos();
            TextFmt("get_cursor_screen_pos(): ({:.0f}, {:.0f})", screenPos.x, screenPos.y);

            Separator();

            auto mouse = GetMousePos();
            TextFmt("get_mouse_pos(): ({:.0f}, {:.0f})", mouse.x, mouse.y);
            TextFmt("is_mouse_down(Left): {}", IsMouseDown(ImGuiMouseButton_Left));

            EndSection();
        }
    }

    static void ShowIconsDemo()
    {
        if (BeginSection("Icons"))
        {
            Text("Placeholder icons (replace with font icons later):");
            Spacing();

            IconButton(Icons::Folder, "Folder");
            SameLine();
            IconButton(Icons::File, "File");
            SameLine();
            IconButton(Icons::Save, "Save");
            SameLine();
            IconButton(Icons::Open, "Open");

            Spacing();

            IconButton(Icons::Play);
            SameLine();
            IconButton(Icons::Pause);
            SameLine();
            IconButton(Icons::Stop);
            SameLine();
            IconButton(Icons::Add);
            SameLine();
            IconButton(Icons::Remove);

            EndSection();
        }
    }

    static void ShowNotificationsDemo()
    {
        if (BeginSection("Notifications"))
        {
            Text("notify() - Toast notifications:");
            Spacing();

            if (Button("Info"))
                Notify("This is an info notification", NotificationType::Info);
            SameLine();
            if (Button("Success"))
                Notify("Operation successful!", NotificationType::Success);
            SameLine();
            if (Button("Warning"))
                Notify("Warning: Check your input", NotificationType::Warning);
            SameLine();
            if (Button("Error"))
                Notify("Error: Something went wrong", NotificationType::Error);

            Spacing();
            Text("Call render_notifications() at end of frame to display.");

            EndSection();
        }
    }

    static void ShowStyleDemo()
    {
        if (BeginSection("Style"))
        {
            Text("push_style_color() / pop_style_color():");
            PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.5f, 0, 1));
            Text("Orange text");
            PopStyleColor();

            Spacing();

            Text("push_style_var() / pop_style_var():");
            PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
            Button("Extra Padding");
            PopStyleVar();

            SameLine();
            Button("Normal Padding");

            EndSection();
        }
    }

    static void ShowChildWindowsDemo()
    {
        if (BeginSection("Child Windows"))
        {
            Text("begin_child() / end_child():");

            // Note: end_child() must ALWAYS be called after begin_child(), unlike begin_window()
            BeginChild("child1", {0, 100}, ImGuiChildFlags_Borders);
            for (int i = 0; i < 20; i++)
                TextFmt("Scrollable item {}", i);
            EndChild();

            Spacing();

            Text("begin_group() / end_group() for layout:");
            BeginGroup();
            Button("Grouped");
            Button("Buttons");
            EndGroup();

            SameLine();

            BeginGroup();
            Text("Grouped");
            Text("Text");
            EndGroup();

            EndSection();
        }
    }

    static void ShowDisabledDemo()
    {
        if (BeginSection("Disabled State"))
        {
            Text("begin_disabled() / end_disabled():");

            BeginDisabled(true);
            Button("Disabled Button");
            static float val = 0.5f;
            ImGui::SliderFloat("Disabled Slider", &val, 0, 1);
            EndDisabled();

            Button("Enabled Button");

            EndSection();
        }
    }

    static void ShowCombosListboxesDemo()
    {
        if (BeginSection("Combos & Listboxes"))
        {
            Text("begin_combo() / end_combo():");

            static int comboItem = 0;
            static const char* items[] = {"Apple", "Banana", "Cherry", "Date"};

            if (BeginCombo("Fruits", items[comboItem]))
            {
                for (int i = 0; i < 4; i++)
                {
                    bool selected = (comboItem == i);
                    if (Selectable(items[i], static_cast<bool>(selected), ImGuiSelectableFlags_None))
                        comboItem = i;
                    if (selected)
                        SetItemDefaultFocus();
                }
                EndCombo();
            }

            Separator();

            Text("begin_listbox() / end_listbox():");

            static int listboxItem = 0;
            if (BeginListbox("##listbox", {-1, 80}))
            {
                for (int i = 0; i < 4; i++)
                {
                    if (Selectable(items[i], listboxItem == i))
                        listboxItem = i;
                }
                EndListbox();
            }

            EndSection();
        }
    }

    void ShowDemoWindow(bool* pOpen)
    {
        if (pOpen && !*pOpen)
            return;

        ImGui::SetNextWindowSize(ImVec2(600, 800), ImGuiCond_FirstUseEver);

        bool isOpen = true;
        bool& openRef = pOpen ? *pOpen : isOpen;
        if (!BeginWindow("Lumina UI Demo", openRef, ImGuiWindowFlags_None))
        {
            EndWindow();
            return;
        }

        Text("Lumina UI Module Demo");
        TextDisabled("All functions use the ui:: namespace");
        Separator();

        ShowTextDemo();
        ShowButtonsDemo();
        ShowPropertiesDemo();
        ShowPanelsDemo();
        ShowLayoutsDemo();
        ShowStyleDemo();
        ShowChildWindowsDemo();
        ShowDisabledDemo();
        ShowTablesDemo();
        ShowTreesDemo();
        ShowMenusDemo();
        ShowToolbarsDemo();
        ShowPopupsDemo();
        ShowTabsDemo();
        ShowCombosListboxesDemo();
        ShowStateDemo();
        ShowIconsDemo();
        ShowNotificationsDemo();

        EndWindow();
    }
}
