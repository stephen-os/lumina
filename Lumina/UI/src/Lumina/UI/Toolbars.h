#pragma once

#include <imgui.h>

#include <concepts>

namespace Lumina::UI
{
    struct ToolbarConfig
    {
        float Height = 32.0f;
        float ButtonSize = 24.0f;
        float Spacing = 4.0f;
    };

    inline void Toolbar(std::invocable auto&& body, const ToolbarConfig& config = {})
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 4.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(config.Spacing, 0.0f));
        ImGui::BeginChild("##toolbar", ImVec2(0, config.Height), ImGuiChildFlags_None,
                          ImGuiWindowFlags_NoScrollbar);
        body();
        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::Separator();
    }

    inline bool ToolbarButton(const char* label, const ImVec2& size = {0, 0})
    {
        bool clicked = ImGui::Button(label, size);
        ImGui::SameLine();
        return clicked;
    }

    inline bool ToolbarToggle(const char* label, bool& active, const ImVec2& size = {0, 0})
    {
        if (active)
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));

        bool clicked = ImGui::Button(label, size);

        if (active)
            ImGui::PopStyleColor();

        if (clicked) active = !active;
        ImGui::SameLine();
        return clicked;
    }

    inline void ToolbarSeparator()
    {
        ImGui::SameLine(0, 8.0f);
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(p.x, p.y + 2.0f),
            ImVec2(p.x, p.y + ImGui::GetFrameHeight() - 2.0f),
            ImGui::GetColorU32(ImGuiCol_Separator),
            1.0f
        );
        ImGui::Dummy(ImVec2(1.0f, 0.0f));
        ImGui::SameLine(0, 8.0f);
    }
}
