#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

namespace lumina::ui
{
    // Style color push/pop
    inline void push_style_color(ImGuiCol idx, const ImVec4& color) { ImGui::PushStyleColor(idx, color); }
    inline void push_style_color(ImGuiCol idx, const glm::vec4& color) { ImGui::PushStyleColor(idx, ImVec4(color.r, color.g, color.b, color.a)); }
    inline void push_style_color(ImGuiCol idx, const glm::vec3& color) { ImGui::PushStyleColor(idx, ImVec4(color.r, color.g, color.b, 1.0f)); }
    inline void push_style_color(ImGuiCol idx, ImU32 color) { ImGui::PushStyleColor(idx, color); }
    inline void pop_style_color(int count = 1) { ImGui::PopStyleColor(count); }

    // Style var push/pop
    inline void push_style_var(ImGuiStyleVar idx, float val) { ImGui::PushStyleVar(idx, val); }
    inline void push_style_var(ImGuiStyleVar idx, const ImVec2& val) { ImGui::PushStyleVar(idx, val); }
    inline void push_style_var(ImGuiStyleVar idx, const glm::vec2& val) { ImGui::PushStyleVar(idx, ImVec2(val.x, val.y)); }
    inline void pop_style_var(int count = 1) { ImGui::PopStyleVar(count); }

    // Font push/pop
    inline void push_font(ImFont* font) { ImGui::PushFont(font); }
    inline void pop_font() { ImGui::PopFont(); }

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

    // Basic widgets
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
