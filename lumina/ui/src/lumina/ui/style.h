#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

namespace Lumina::UI
{
    // Style color push/pop
    inline void PushStyleColor(ImGuiCol idx, const ImVec4& color) { ImGui::PushStyleColor(idx, color); }
    inline void PushStyleColor(ImGuiCol idx, const glm::vec4& color) { ImGui::PushStyleColor(idx, ImVec4(color.r, color.g, color.b, color.a)); }
    inline void PushStyleColor(ImGuiCol idx, const glm::vec3& color) { ImGui::PushStyleColor(idx, ImVec4(color.r, color.g, color.b, 1.0f)); }
    inline void PushStyleColor(ImGuiCol idx, ImU32 color) { ImGui::PushStyleColor(idx, color); }
    inline void PopStyleColor(int count = 1) { ImGui::PopStyleColor(count); }

    // Style var push/pop
    inline void PushStyleVar(ImGuiStyleVar idx, float val) { ImGui::PushStyleVar(idx, val); }
    inline void PushStyleVar(ImGuiStyleVar idx, const ImVec2& val) { ImGui::PushStyleVar(idx, val); }
    inline void PushStyleVar(ImGuiStyleVar idx, const glm::vec2& val) { ImGui::PushStyleVar(idx, ImVec2(val.x, val.y)); }
    inline void PopStyleVar(int count = 1) { ImGui::PopStyleVar(count); }

    // Font push/pop
    inline void PushFont(ImFont* font) { ImGui::PushFont(font); }
    inline void PopFont() { ImGui::PopFont(); }

    // Themed button variants
    inline bool Button(const char* label)
    {
        return ImGui::Button(label);
    }

    inline bool Button(const char* label, const glm::vec2& size)
    {
        return ImGui::Button(label, ImVec2(size.x, size.y));
    }

    inline bool Button(const char* label, float width, float height)
    {
        return ImGui::Button(label, ImVec2(width, height));
    }

    inline bool ButtonSmall(const char* label)
    {
        return ImGui::SmallButton(label);
    }

    inline bool ButtonPrimary(const char* label)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.5f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.6f, 0.3f, 1.0f));
        bool clicked = ImGui::Button(label);
        ImGui::PopStyleColor(3);
        return clicked;
    }

    inline bool ButtonPrimary(const char* label, const glm::vec2& size)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.5f, 0.0f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.5f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.6f, 0.3f, 1.0f));
        bool clicked = ImGui::Button(label, ImVec2(size.x, size.y));
        ImGui::PopStyleColor(3);
        return clicked;
    }

    inline bool ButtonDanger(const char* label)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.15f, 0.15f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.25f, 0.25f, 1.0f));
        bool clicked = ImGui::Button(label);
        ImGui::PopStyleColor(3);
        return clicked;
    }

    inline bool ButtonDanger(const char* label, const glm::vec2& size)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.15f, 0.15f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.25f, 0.25f, 1.0f));
        bool clicked = ImGui::Button(label, ImVec2(size.x, size.y));
        ImGui::PopStyleColor(3);
        return clicked;
    }

    inline bool ButtonSuccess(const char* label)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.6f, 0.15f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.7f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.8f, 0.25f, 1.0f));
        bool clicked = ImGui::Button(label);
        ImGui::PopStyleColor(3);
        return clicked;
    }

    inline bool ButtonSuccess(const char* label, const glm::vec2& size)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.6f, 0.15f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.7f, 0.2f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.8f, 0.25f, 1.0f));
        bool clicked = ImGui::Button(label, ImVec2(size.x, size.y));
        ImGui::PopStyleColor(3);
        return clicked;
    }

    // Basic widgets
    inline bool Checkbox(const char* label, bool& value)
    {
        return ImGui::Checkbox(label, &value);
    }

    inline bool Selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0)
    {
        return ImGui::Selectable(label, selected, flags);
    }

    inline bool Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const glm::vec2& size)
    {
        return ImGui::Selectable(label, selected, flags, ImVec2(size.x, size.y));
    }

    inline bool Selectable(const char* label, bool& selected, ImGuiSelectableFlags flags = 0)
    {
        return ImGui::Selectable(label, &selected, flags);
    }

    inline bool Selectable(const char* label, bool& selected, ImGuiSelectableFlags flags, const glm::vec2& size)
    {
        return ImGui::Selectable(label, &selected, flags, ImVec2(size.x, size.y));
    }
}
