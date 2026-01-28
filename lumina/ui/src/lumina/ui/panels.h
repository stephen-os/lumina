#pragma once

#include <imgui.h>

#include <concepts>

namespace lumina::ui
{
    inline bool panel(const char* title, std::invocable auto&& body, ImGuiWindowFlags flags = 0)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
        bool visible = ImGui::Begin(title, nullptr, flags);
        ImGui::PopStyleVar();
        if (visible) body();
        ImGui::End();
        return visible;
    }

    inline bool panel(const char* title, bool& open, std::invocable auto&& body, ImGuiWindowFlags flags = 0)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
        bool visible = ImGui::Begin(title, &open, flags);
        ImGui::PopStyleVar();
        if (visible) body();
        ImGui::End();
        return visible;
    }

    inline bool section(const char* label, std::invocable auto&& body,
                        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen)
    {
        bool open = ImGui::CollapsingHeader(label, flags);
        if (open)
        {
            ImGui::Indent();
            ImGui::Spacing();
            body();
            ImGui::Spacing();
            ImGui::Unindent();
        }
        return open;
    }

    inline bool section(const char* label, bool& visible, std::invocable auto&& body,
                        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen)
    {
        bool open = ImGui::CollapsingHeader(label, &visible, flags);
        if (open)
        {
            ImGui::Indent();
            ImGui::Spacing();
            body();
            ImGui::Spacing();
            ImGui::Unindent();
        }
        return open;
    }
}
