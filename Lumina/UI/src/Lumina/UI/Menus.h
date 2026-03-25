#pragma once

#include <imgui.h>

#include <concepts>

namespace Lumina::UI
{
    // MenuBar(), MainMenuBar(), Menu() are in scoped.h
    // This file provides menu item helpers.

    inline bool MenuItem(const char* label, const char* shortcut = nullptr,
                          bool selected = false, bool enabled = true)
    {
        return ImGui::MenuItem(label, shortcut, selected, enabled);
    }

    inline bool MenuItem(const char* label, bool& selected, const char* shortcut = nullptr, bool enabled = true)
    {
        return ImGui::MenuItem(label, shortcut, &selected, enabled);
    }

    inline void MenuSeparator()
    {
        ImGui::Separator();
    }

    inline bool ContextMenu(std::invocable auto&& body, const char* strId = nullptr)
    {
        bool open = ImGui::BeginPopupContextItem(strId);
        if (open)
        {
            body();
            ImGui::EndPopup();
        }
        return open;
    }

    inline bool ContextMenuWindow(std::invocable auto&& body, const char* strId = nullptr)
    {
        bool open = ImGui::BeginPopupContextWindow(strId);
        if (open)
        {
            body();
            ImGui::EndPopup();
        }
        return open;
    }
}
