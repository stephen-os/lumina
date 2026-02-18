#pragma once

#include <imgui.h>

namespace lumina::ui
{
    // Panel - a styled window with consistent padding
    inline bool begin_panel(const char* title, ImGuiWindowFlags flags = 0)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
        bool visible = ImGui::Begin(title, nullptr, flags);
        ImGui::PopStyleVar();
        return visible;
    }

    inline bool begin_panel(const char* title, bool& open, ImGuiWindowFlags flags = 0)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
        bool visible = ImGui::Begin(title, &open, flags);
        ImGui::PopStyleVar();
        return visible;
    }

    inline void end_panel()
    {
        ImGui::End();
    }

    // Section - collapsing header with indented content
    // Returns true if the section is open (content should be rendered)
    inline bool begin_section(const char* label, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen)
    {
        bool open = ImGui::CollapsingHeader(label, flags);
        if (open)
        {
            ImGui::Indent();
            ImGui::Spacing();
        }
        return open;
    }

    inline bool begin_section(const char* label, bool& visible, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen)
    {
        bool open = ImGui::CollapsingHeader(label, &visible, flags);
        if (open)
        {
            ImGui::Indent();
            ImGui::Spacing();
        }
        return open;
    }

    // Call only if begin_section returned true
    inline void end_section()
    {
        ImGui::Spacing();
        ImGui::Unindent();
    }
}
