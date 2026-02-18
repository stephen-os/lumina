#pragma once

#include <imgui.h>

namespace lumina::ui
{
    // --- Window ---
    inline bool begin_window(const char* title, ImGuiWindowFlags flags = 0)
    {
        return ImGui::Begin(title, nullptr, flags);
    }

    inline bool begin_window(const char* title, bool& open, ImGuiWindowFlags flags = 0)
    {
        return ImGui::Begin(title, &open, flags);
    }

    inline void end_window()
    {
        ImGui::End();
    }

    // --- Child ---
    inline bool begin_child(const char* id, const ImVec2& size = {0, 0},
                            ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0)
    {
        return ImGui::BeginChild(id, size, child_flags, window_flags);
    }

    inline void end_child()
    {
        ImGui::EndChild();
    }

    // --- Group ---
    inline void begin_group()
    {
        ImGui::BeginGroup();
    }

    inline void end_group()
    {
        ImGui::EndGroup();
    }

    // --- Tree node ---
    inline bool begin_tree_node(const char* label, ImGuiTreeNodeFlags flags = 0)
    {
        return ImGui::TreeNodeEx(label, flags);
    }

    inline void end_tree_node()
    {
        ImGui::TreePop();
    }

    // --- Popup ---
    inline bool begin_popup(const char* id, ImGuiWindowFlags flags = 0)
    {
        return ImGui::BeginPopup(id, flags);
    }

    inline bool begin_popup_modal(const char* title, ImGuiWindowFlags flags = 0)
    {
        return ImGui::BeginPopupModal(title, nullptr, flags);
    }

    inline bool begin_popup_modal(const char* title, bool& open, ImGuiWindowFlags flags = 0)
    {
        return ImGui::BeginPopupModal(title, &open, flags);
    }

    inline bool begin_popup_context_item(const char* str_id = nullptr,
                                         ImGuiPopupFlags popup_flags = ImGuiPopupFlags_MouseButtonRight)
    {
        return ImGui::BeginPopupContextItem(str_id, popup_flags);
    }

    inline bool begin_popup_context_window(const char* str_id = nullptr,
                                           ImGuiPopupFlags popup_flags = ImGuiPopupFlags_MouseButtonRight)
    {
        return ImGui::BeginPopupContextWindow(str_id, popup_flags);
    }

    inline void end_popup()
    {
        ImGui::EndPopup();
    }

    inline void open_popup(const char* id, ImGuiPopupFlags flags = 0)
    {
        ImGui::OpenPopup(id, flags);
    }

    inline void close_current_popup()
    {
        ImGui::CloseCurrentPopup();
    }

    // --- Tab bar ---
    inline bool begin_tab_bar(const char* id, ImGuiTabBarFlags flags = 0)
    {
        return ImGui::BeginTabBar(id, flags);
    }

    inline void end_tab_bar()
    {
        ImGui::EndTabBar();
    }

    inline bool begin_tab_item(const char* label, ImGuiTabItemFlags flags = 0)
    {
        return ImGui::BeginTabItem(label, nullptr, flags);
    }

    inline bool begin_tab_item(const char* label, bool& open, ImGuiTabItemFlags flags = 0)
    {
        return ImGui::BeginTabItem(label, &open, flags);
    }

    inline void end_tab_item()
    {
        ImGui::EndTabItem();
    }

    // --- Table ---
    inline bool begin_table(const char* id, int columns, ImGuiTableFlags flags = 0,
                            const ImVec2& outer_size = {0, 0}, float inner_width = 0.0f)
    {
        return ImGui::BeginTable(id, columns, flags, outer_size, inner_width);
    }

    inline void end_table()
    {
        ImGui::EndTable();
    }

    // --- Menu bar ---
    inline bool begin_menu_bar()
    {
        return ImGui::BeginMenuBar();
    }

    inline void end_menu_bar()
    {
        ImGui::EndMenuBar();
    }

    inline bool begin_main_menu_bar()
    {
        return ImGui::BeginMainMenuBar();
    }

    inline void end_main_menu_bar()
    {
        ImGui::EndMainMenuBar();
    }

    inline bool begin_menu(const char* label, bool enabled = true)
    {
        return ImGui::BeginMenu(label, enabled);
    }

    inline void end_menu()
    {
        ImGui::EndMenu();
    }

    // --- Tooltip ---
    inline bool begin_tooltip()
    {
        return ImGui::BeginTooltip();
    }

    inline void end_tooltip()
    {
        ImGui::EndTooltip();
    }

    // --- Combo ---
    inline bool begin_combo(const char* label, const char* preview, ImGuiComboFlags flags = 0)
    {
        return ImGui::BeginCombo(label, preview, flags);
    }

    inline void end_combo()
    {
        ImGui::EndCombo();
    }

    // --- Listbox ---
    inline bool begin_listbox(const char* label, const ImVec2& size = {0, 0})
    {
        return ImGui::BeginListBox(label, size);
    }

    inline void end_listbox()
    {
        ImGui::EndListBox();
    }

    // --- Drag/drop ---
    inline bool begin_drag_drop_source(ImGuiDragDropFlags flags = 0)
    {
        return ImGui::BeginDragDropSource(flags);
    }

    inline void end_drag_drop_source()
    {
        ImGui::EndDragDropSource();
    }

    inline bool begin_drag_drop_target()
    {
        return ImGui::BeginDragDropTarget();
    }

    inline void end_drag_drop_target()
    {
        ImGui::EndDragDropTarget();
    }

    // --- Disabled ---
    inline void begin_disabled(bool disabled = true)
    {
        ImGui::BeginDisabled(disabled);
    }

    inline void end_disabled()
    {
        ImGui::EndDisabled();
    }
}
