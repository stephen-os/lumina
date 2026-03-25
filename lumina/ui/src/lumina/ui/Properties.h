#pragma once

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <concepts>
#include <cstring>
#include <span>
#include <string>

namespace Lumina::UI
{
    inline constexpr float DefaultLabelWidth = 120.0f;

    namespace Detail
    {
        inline void BeginProperty(const char* label, float labelWidth = DefaultLabelWidth)
        {
            ImGui::PushID(label);
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, labelWidth);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(label);
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(-1);
        }

        inline void EndProperty()
        {
            ImGui::Columns(1);
            ImGui::PopID();
        }
    }

    // --- Float ---

    inline bool Property(const char* label, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                         const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::DragFloat("##v", &value, speed, min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- Int ---

    inline bool Property(const char* label, int& value, float speed = 1.0f, int min = 0, int max = 0,
                         const char* format = "%d")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::DragInt("##v", &value, speed, min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- Bool ---

    inline bool Property(const char* label, bool& value)
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::Checkbox("##v", &value);
        Detail::EndProperty();
        return changed;
    }

    // --- String ---

    inline bool Property(const char* label, char* buffer, size_t bufferSize, ImGuiInputTextFlags flags = 0)
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::InputText("##v", buffer, bufferSize, flags);
        Detail::EndProperty();
        return changed;
    }

    inline bool Property(const char* label, std::string& value, ImGuiInputTextFlags flags = 0)
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::InputText("##v", &value, flags);
        Detail::EndProperty();
        return changed;
    }

    // --- Vec2 ---

    inline bool PropertyVec2(const char* label, float v[2], float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                              const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::DragFloat2("##v", v, speed, min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- Vec3 ---

    inline bool PropertyVec3(const char* label, float v[3], float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                              const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::DragFloat3("##v", v, speed, min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- Vec4 ---

    inline bool PropertyVec4(const char* label, float v[4], float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                              const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::DragFloat4("##v", v, speed, min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- GLM vec2 ---

    inline bool Property(const char* label, glm::vec2& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                         const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::DragFloat2("##v", glm::value_ptr(value), speed, min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- GLM vec3 ---

    inline bool Property(const char* label, glm::vec3& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                         const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::DragFloat3("##v", glm::value_ptr(value), speed, min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- GLM vec4 ---

    inline bool Property(const char* label, glm::vec4& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                         const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::DragFloat4("##v", glm::value_ptr(value), speed, min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- GLM color (vec3 as RGB) ---

    inline bool PropertyColor(const char* label, glm::vec3& color, ImGuiColorEditFlags flags = 0)
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::ColorEdit3("##v", glm::value_ptr(color), flags);
        Detail::EndProperty();
        return changed;
    }

    // --- GLM color (vec4 as RGBA) ---

    inline bool PropertyColor(const char* label, glm::vec4& color, ImGuiColorEditFlags flags = 0)
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::ColorEdit4("##v", glm::value_ptr(color), flags);
        Detail::EndProperty();
        return changed;
    }

    // --- GLM slider vec2 ---

    inline bool PropertySlider(const char* label, glm::vec2& value, float min, float max,
                                const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::SliderFloat2("##v", glm::value_ptr(value), min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- GLM slider vec3 ---

    inline bool PropertySlider(const char* label, glm::vec3& value, float min, float max,
                                const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::SliderFloat3("##v", glm::value_ptr(value), min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- GLM slider vec4 ---

    inline bool PropertySlider(const char* label, glm::vec4& value, float min, float max,
                                const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::SliderFloat4("##v", glm::value_ptr(value), min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- Slider ---

    inline bool PropertySlider(const char* label, float& value, float min, float max,
                                const char* format = "%.3f")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::SliderFloat("##v", &value, min, max, format);
        Detail::EndProperty();
        return changed;
    }

    inline bool PropertySlider(const char* label, int& value, int min, int max,
                                const char* format = "%d")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::SliderInt("##v", &value, min, max, format);
        Detail::EndProperty();
        return changed;
    }

    // --- Color ---

    inline bool PropertyColor3(const char* label, float color[3], ImGuiColorEditFlags flags = 0)
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::ColorEdit3("##v", color, flags);
        Detail::EndProperty();
        return changed;
    }

    inline bool PropertyColor4(const char* label, float color[4], ImGuiColorEditFlags flags = 0)
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::ColorEdit4("##v", color, flags);
        Detail::EndProperty();
        return changed;
    }

    // --- Dropdown ---

    inline bool PropertyDropdown(const char* label, int& current, const char* const items[], int itemCount)
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::Combo("##v", &current, items, itemCount);
        Detail::EndProperty();
        return changed;
    }

    inline bool PropertyDropdown(const char* label, int& current, const char* itemsSeparatedByZeros)
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::Combo("##v", &current, itemsSeparatedByZeros);
        Detail::EndProperty();
        return changed;
    }

    inline bool PropertyDropdown(const char* label, int& current, std::span<const char* const> items)
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::Combo("##v", &current, items.data(), static_cast<int>(items.size()));
        Detail::EndProperty();
        return changed;
    }

    // --- Angle (degrees) ---

    inline bool PropertyAngle(const char* label, float& radians, float minDeg = -360.0f, float maxDeg = 360.0f,
                               const char* format = "%.0f deg")
    {
        Detail::BeginProperty(label);
        bool changed = ImGui::SliderAngle("##v", &radians, minDeg, maxDeg, format);
        Detail::EndProperty();
        return changed;
    }

    // --- Custom widget ---

    template<std::invocable F>
    requires std::same_as<std::invoke_result_t<F>, bool>
    inline bool PropertyCustom(const char* label, F&& widgetFn)
    {
        Detail::BeginProperty(label);
        bool changed = widgetFn();
        Detail::EndProperty();
        return changed;
    }

    // --- Read-only text property ---

    inline void PropertyText(const char* label, const char* value)
    {
        Detail::BeginProperty(label);
        ImGui::TextUnformatted(value);
        Detail::EndProperty();
    }
}
