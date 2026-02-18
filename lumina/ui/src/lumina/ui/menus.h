#pragma once

#include <imgui.h>

#include <concepts>

namespace lumina::ui
{
    // menu_bar(), main_menu_bar(), menu() are in scoped.h
    // This file provides menu item helpers.

    inline bool menu_item(const char* label, const char* shortcut = nullptr,
                          bool selected = false, bool enabled = true)
    {
        return ImGui::MenuItem(label, shortcut, selected, enabled);
    }

    inline bool menu_item(const char* label, bool& selected, const char* shortcut = nullptr, bool enabled = true)
    {
        return ImGui::MenuItem(label, shortcut, &selected, enabled);
    }

    inline void menu_separator()
    {
        ImGui::Separator();
    }

    inline bool context_menu(std::invocable auto&& body, const char* str_id = nullptr)
    {
        bool open = ImGui::BeginPopupContextItem(str_id);
        if (open)
        {
            body();
            ImGui::EndPopup();
        }
        return open;
    }

    inline bool context_menu_window(std::invocable auto&& body, const char* str_id = nullptr)
    {
        bool open = ImGui::BeginPopupContextWindow(str_id);
        if (open)
        {
            body();
            ImGui::EndPopup();
        }
        return open;
    }
}
