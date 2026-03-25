#pragma once

#include <imgui.h>

namespace Lumina::UI
{
    // --- Window ---
    inline bool BeginWindow(const char* title, ImGuiWindowFlags flags = 0)
    {
        return ImGui::Begin(title, nullptr, flags);
    }

    inline bool BeginWindow(const char* title, bool& open, ImGuiWindowFlags flags = 0)
    {
        return ImGui::Begin(title, &open, flags);
    }

    inline void EndWindow()
    {
        ImGui::End();
    }

    // --- Child ---
    inline bool BeginChild(const char* id, const ImVec2& size = {0, 0},
                            ImGuiChildFlags childFlags = 0, ImGuiWindowFlags windowFlags = 0)
    {
        return ImGui::BeginChild(id, size, childFlags, windowFlags);
    }

    inline void EndChild()
    {
        ImGui::EndChild();
    }

    // --- Group ---
    inline void BeginGroup()
    {
        ImGui::BeginGroup();
    }

    inline void EndGroup()
    {
        ImGui::EndGroup();
    }

    // --- Tree node ---
    inline bool BeginTreeNode(const char* label, ImGuiTreeNodeFlags flags = 0)
    {
        return ImGui::TreeNodeEx(label, flags);
    }

    inline void EndTreeNode()
    {
        ImGui::TreePop();
    }

    // --- Popup ---
    inline bool BeginPopup(const char* id, ImGuiWindowFlags flags = 0)
    {
        return ImGui::BeginPopup(id, flags);
    }

    inline bool BeginPopupModal(const char* title, ImGuiWindowFlags flags = 0)
    {
        return ImGui::BeginPopupModal(title, nullptr, flags);
    }

    inline bool BeginPopupModal(const char* title, bool& open, ImGuiWindowFlags flags = 0)
    {
        return ImGui::BeginPopupModal(title, &open, flags);
    }

    inline bool BeginPopupContextItem(const char* strId = nullptr,
                                         ImGuiPopupFlags popupFlags = ImGuiPopupFlags_MouseButtonRight)
    {
        return ImGui::BeginPopupContextItem(strId, popupFlags);
    }

    inline bool BeginPopupContextWindow(const char* strId = nullptr,
                                           ImGuiPopupFlags popupFlags = ImGuiPopupFlags_MouseButtonRight)
    {
        return ImGui::BeginPopupContextWindow(strId, popupFlags);
    }

    inline void EndPopup()
    {
        ImGui::EndPopup();
    }

    inline void OpenPopup(const char* id, ImGuiPopupFlags flags = 0)
    {
        ImGui::OpenPopup(id, flags);
    }

    inline void CloseCurrentPopup()
    {
        ImGui::CloseCurrentPopup();
    }

    // --- Tab bar ---
    inline bool BeginTabBar(const char* id, ImGuiTabBarFlags flags = 0)
    {
        return ImGui::BeginTabBar(id, flags);
    }

    inline void EndTabBar()
    {
        ImGui::EndTabBar();
    }

    inline bool BeginTabItem(const char* label, ImGuiTabItemFlags flags = 0)
    {
        return ImGui::BeginTabItem(label, nullptr, flags);
    }

    inline bool BeginTabItem(const char* label, bool& open, ImGuiTabItemFlags flags = 0)
    {
        return ImGui::BeginTabItem(label, &open, flags);
    }

    inline void EndTabItem()
    {
        ImGui::EndTabItem();
    }

    // --- Table ---
    inline bool BeginTable(const char* id, int columns, ImGuiTableFlags flags = 0,
                            const ImVec2& outerSize = {0, 0}, float innerWidth = 0.0f)
    {
        return ImGui::BeginTable(id, columns, flags, outerSize, innerWidth);
    }

    inline void EndTable()
    {
        ImGui::EndTable();
    }

    // --- Menu bar ---
    inline bool BeginMenuBar()
    {
        return ImGui::BeginMenuBar();
    }

    inline void EndMenuBar()
    {
        ImGui::EndMenuBar();
    }

    inline bool BeginMainMenuBar()
    {
        return ImGui::BeginMainMenuBar();
    }

    inline void EndMainMenuBar()
    {
        ImGui::EndMainMenuBar();
    }

    inline bool BeginMenu(const char* label, bool enabled = true)
    {
        return ImGui::BeginMenu(label, enabled);
    }

    inline void EndMenu()
    {
        ImGui::EndMenu();
    }

    // --- Tooltip ---
    inline bool BeginTooltip()
    {
        return ImGui::BeginTooltip();
    }

    inline void EndTooltip()
    {
        ImGui::EndTooltip();
    }

    // --- Combo ---
    inline bool BeginCombo(const char* label, const char* preview, ImGuiComboFlags flags = 0)
    {
        return ImGui::BeginCombo(label, preview, flags);
    }

    inline void EndCombo()
    {
        ImGui::EndCombo();
    }

    // --- Listbox ---
    inline bool BeginListbox(const char* label, const ImVec2& size = {0, 0})
    {
        return ImGui::BeginListBox(label, size);
    }

    inline void EndListbox()
    {
        ImGui::EndListBox();
    }

    // --- Drag/drop ---
    inline bool BeginDragDropSource(ImGuiDragDropFlags flags = 0)
    {
        return ImGui::BeginDragDropSource(flags);
    }

    inline void EndDragDropSource()
    {
        ImGui::EndDragDropSource();
    }

    inline bool BeginDragDropTarget()
    {
        return ImGui::BeginDragDropTarget();
    }

    inline void EndDragDropTarget()
    {
        ImGui::EndDragDropTarget();
    }

    // --- Disabled ---
    inline void BeginDisabled(bool disabled = true)
    {
        ImGui::BeginDisabled(disabled);
    }

    inline void EndDisabled()
    {
        ImGui::EndDisabled();
    }
}
