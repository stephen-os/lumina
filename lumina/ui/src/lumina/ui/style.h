#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

#include <concepts>

namespace lumina::ui
{
    inline void with_style_color(ImGuiCol idx, const ImVec4& color, std::invocable auto&& body)
    {
        ImGui::PushStyleColor(idx, color);
        body();
        ImGui::PopStyleColor();
    }

    inline void with_style_color(ImGuiCol idx, const glm::vec4& color, std::invocable auto&& body)
    {
        ImGui::PushStyleColor(idx, ImVec4(color.r, color.g, color.b, color.a));
        body();
        ImGui::PopStyleColor();
    }

    inline void with_style_color(ImGuiCol idx, const glm::vec3& color, std::invocable auto&& body)
    {
        ImGui::PushStyleColor(idx, ImVec4(color.r, color.g, color.b, 1.0f));
        body();
        ImGui::PopStyleColor();
    }

    inline void with_style_var(ImGuiStyleVar idx, const glm::vec2& val, std::invocable auto&& body)
    {
        ImGui::PushStyleVar(idx, ImVec2(val.x, val.y));
        body();
        ImGui::PopStyleVar();
    }

    inline void with_style_var(ImGuiStyleVar idx, float val, std::invocable auto&& body)
    {
        ImGui::PushStyleVar(idx, val);
        body();
        ImGui::PopStyleVar();
    }

    inline void with_style_var(ImGuiStyleVar idx, const ImVec2& val, std::invocable auto&& body)
    {
        ImGui::PushStyleVar(idx, val);
        body();
        ImGui::PopStyleVar();
    }

    inline void with_font(ImFont* font, std::invocable auto&& body)
    {
        ImGui::PushFont(font);
        body();
        ImGui::PopFont();
    }

    // Themed button variants

    inline bool button(const char* label, const ImVec2& size = {0, 0})
    {
        return ImGui::Button(label, size);
    }

    inline bool button_small(const char* label)
    {
        return ImGui::SmallButton(label);
    }

    inline bool button_primary(const char* label, const ImVec2& size = {0, 0})
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.5f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.6f, 0.3f, 1.0f));
        bool clicked = ImGui::Button(label, size);
        ImGui::PopStyleColor(3);
        return clicked;
    }

    inline bool button_danger(const char* label, const ImVec2& size = {0, 0})
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.15f, 0.15f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.25f, 0.25f, 1.0f));
        bool clicked = ImGui::Button(label, size);
        ImGui::PopStyleColor(3);
        return clicked;
    }

    inline bool button_success(const char* label, const ImVec2& size = {0, 0})
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.6f, 0.15f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.7f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.8f, 0.25f, 1.0f));
        bool clicked = ImGui::Button(label, size);
        ImGui::PopStyleColor(3);
        return clicked;
    }

    inline bool checkbox(const char* label, bool& value)
    {
        return ImGui::Checkbox(label, &value);
    }

    inline bool selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = {0, 0})
    {
        return ImGui::Selectable(label, selected, flags, size);
    }

    inline bool selectable(const char* label, bool& selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = {0, 0})
    {
        return ImGui::Selectable(label, &selected, flags, size);
    }
}
