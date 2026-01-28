#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <concepts>

namespace lumina::ui
{
    // --- Window ---

    inline bool window(const char* title, std::invocable auto&& body, ImGuiWindowFlags flags = 0)
    {
        bool visible = ImGui::Begin(title, nullptr, flags);
        if (visible) body();
        ImGui::End();
        return visible;
    }

    inline bool window(const char* title, bool& open, std::invocable auto&& body, ImGuiWindowFlags flags = 0)
    {
        bool visible = ImGui::Begin(title, &open, flags);
        if (visible) body();
        ImGui::End();
        return visible;
    }

    // --- Child ---

    inline bool child(const char* id, std::invocable auto&& body, const ImVec2& size = {0, 0},
                      ImGuiChildFlags child_flags = 0, ImGuiWindowFlags window_flags = 0)
    {
        bool visible = ImGui::BeginChild(id, size, child_flags, window_flags);
        if (visible) body();
        ImGui::EndChild();
        return visible;
    }

    // --- Group ---

    inline void group(std::invocable auto&& body)
    {
        ImGui::BeginGroup();
        body();
        ImGui::EndGroup();
    }

    // --- Tree node ---

    inline bool tree_node(const char* label, std::invocable auto&& body, ImGuiTreeNodeFlags flags = 0)
    {
        bool open = ImGui::TreeNodeEx(label, flags);
        if (open)
        {
            body();
            ImGui::TreePop();
        }
        return open;
    }

    // --- Collapsing header ---

    inline bool collapsing_header(const char* label, std::invocable auto&& body, ImGuiTreeNodeFlags flags = 0)
    {
        bool open = ImGui::CollapsingHeader(label, flags);
        if (open) body();
        return open;
    }

    // --- Popup ---

    inline bool popup(const char* id, std::invocable auto&& body, ImGuiWindowFlags flags = 0)
    {
        bool open = ImGui::BeginPopup(id, flags);
        if (open)
        {
            body();
            ImGui::EndPopup();
        }
        return open;
    }

    inline bool popup_modal(const char* title, std::invocable auto&& body, ImGuiWindowFlags flags = 0)
    {
        bool open = ImGui::BeginPopupModal(title, nullptr, flags);
        if (open)
        {
            body();
            ImGui::EndPopup();
        }
        return open;
    }

    inline bool popup_modal(const char* title, bool& open, std::invocable auto&& body, ImGuiWindowFlags flags = 0)
    {
        bool visible = ImGui::BeginPopupModal(title, &open, flags);
        if (visible)
        {
            body();
            ImGui::EndPopup();
        }
        return visible;
    }

    inline bool popup_context_item(std::invocable auto&& body, const char* str_id = nullptr,
                                   ImGuiPopupFlags popup_flags = ImGuiPopupFlags_MouseButtonRight)
    {
        bool open = ImGui::BeginPopupContextItem(str_id, popup_flags);
        if (open)
        {
            body();
            ImGui::EndPopup();
        }
        return open;
    }

    inline bool popup_context_window(std::invocable auto&& body, const char* str_id = nullptr,
                                     ImGuiPopupFlags popup_flags = ImGuiPopupFlags_MouseButtonRight)
    {
        bool open = ImGui::BeginPopupContextWindow(str_id, popup_flags);
        if (open)
        {
            body();
            ImGui::EndPopup();
        }
        return open;
    }

    // --- Tab bar ---

    inline bool tab_bar(const char* id, std::invocable auto&& body, ImGuiTabBarFlags flags = 0)
    {
        bool open = ImGui::BeginTabBar(id, flags);
        if (open)
        {
            body();
            ImGui::EndTabBar();
        }
        return open;
    }

    inline bool tab_item(const char* label, std::invocable auto&& body, ImGuiTabItemFlags flags = 0)
    {
        bool open = ImGui::BeginTabItem(label, nullptr, flags);
        if (open)
        {
            body();
            ImGui::EndTabItem();
        }
        return open;
    }

    inline bool tab_item(const char* label, bool& open, std::invocable auto&& body, ImGuiTabItemFlags flags = 0)
    {
        bool visible = ImGui::BeginTabItem(label, &open, flags);
        if (visible)
        {
            body();
            ImGui::EndTabItem();
        }
        return visible;
    }

    // --- Table ---

    inline bool table(const char* id, int columns, std::invocable auto&& body,
                      ImGuiTableFlags flags = 0, const ImVec2& outer_size = {0, 0}, float inner_width = 0.0f)
    {
        bool open = ImGui::BeginTable(id, columns, flags, outer_size, inner_width);
        if (open)
        {
            body();
            ImGui::EndTable();
        }
        return open;
    }

    // --- Menu bar ---

    inline bool menu_bar(std::invocable auto&& body)
    {
        bool open = ImGui::BeginMenuBar();
        if (open)
        {
            body();
            ImGui::EndMenuBar();
        }
        return open;
    }

    inline bool main_menu_bar(std::invocable auto&& body)
    {
        bool open = ImGui::BeginMainMenuBar();
        if (open)
        {
            body();
            ImGui::EndMainMenuBar();
        }
        return open;
    }

    inline bool menu(const char* label, std::invocable auto&& body, bool enabled = true)
    {
        bool open = ImGui::BeginMenu(label, enabled);
        if (open)
        {
            body();
            ImGui::EndMenu();
        }
        return open;
    }

    // --- Tooltip ---

    inline void tooltip(std::invocable auto&& body)
    {
        if (ImGui::BeginTooltip())
        {
            body();
            ImGui::EndTooltip();
        }
    }

    // --- Combo ---

    inline bool combo(const char* label, const char* preview, std::invocable auto&& body, ImGuiComboFlags flags = 0)
    {
        bool open = ImGui::BeginCombo(label, preview, flags);
        if (open)
        {
            body();
            ImGui::EndCombo();
        }
        return open;
    }

    // --- Listbox ---

    inline bool listbox(const char* label, std::invocable auto&& body, const ImVec2& size = {0, 0})
    {
        bool open = ImGui::BeginListBox(label, size);
        if (open)
        {
            body();
            ImGui::EndListBox();
        }
        return open;
    }

    // --- Drag/drop ---

    inline bool drag_drop_source(std::invocable auto&& body, ImGuiDragDropFlags flags = 0)
    {
        bool open = ImGui::BeginDragDropSource(flags);
        if (open)
        {
            body();
            ImGui::EndDragDropSource();
        }
        return open;
    }

    inline bool drag_drop_target(std::invocable auto&& body)
    {
        bool open = ImGui::BeginDragDropTarget();
        if (open)
        {
            body();
            ImGui::EndDragDropTarget();
        }
        return open;
    }

    // --- Disabled ---

    inline void disabled(bool is_disabled, std::invocable auto&& body)
    {
        ImGui::BeginDisabled(is_disabled);
        body();
        ImGui::EndDisabled();
    }
}
