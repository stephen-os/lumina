#pragma once

#include <imgui.h>

#include <concepts>

namespace Lumina::UI
{
    // TreeNode() with lambda body is in scoped.h.
    // This file adds selection-aware variants.

    inline bool TreeNodeLeaf(const char* label, bool selected = false, ImGuiTreeNodeFlags extraFlags = 0)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf
                                 | ImGuiTreeNodeFlags_NoTreePushOnOpen
                                 | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (selected) flags |= ImGuiTreeNodeFlags_Selected;
        flags |= extraFlags;
        ImGui::TreeNodeEx(label, flags);
        return ImGui::IsItemClicked();
    }

    inline bool TreeNodeSelectable(const char* label, bool selected, std::invocable auto&& body,
                                     ImGuiTreeNodeFlags extraFlags = 0)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
                                 | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (selected) flags |= ImGuiTreeNodeFlags_Selected;
        flags |= extraFlags;

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
