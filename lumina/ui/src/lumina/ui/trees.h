#pragma once

#include <imgui.h>

#include <concepts>

namespace lumina::ui
{
    // tree_node() with lambda body is in scoped.h.
    // This file adds selection-aware variants.

    inline bool tree_node_leaf(const char* label, bool selected = false, ImGuiTreeNodeFlags extra_flags = 0)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf
                                 | ImGuiTreeNodeFlags_NoTreePushOnOpen
                                 | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (selected) flags |= ImGuiTreeNodeFlags_Selected;
        flags |= extra_flags;
        ImGui::TreeNodeEx(label, flags);
        return ImGui::IsItemClicked();
    }

    inline bool tree_node_selectable(const char* label, bool selected, std::invocable auto&& body,
                                     ImGuiTreeNodeFlags extra_flags = 0)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                                 | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (selected) flags |= ImGuiTreeNodeFlags_Selected;
        flags |= extra_flags;

        bool open = ImGui::TreeNodeEx(label, flags);
        bool clicked = ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen();
        if (open)
        {
            body();
            ImGui::TreePop();
        }
        return clicked;
    }
}
