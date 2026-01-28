#pragma once

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <concepts>
#include <cstring>
#include <span>
#include <string>

namespace lumina::ui
{
    inline constexpr float default_label_width = 120.0f;

    namespace detail
    {
        inline void begin_property(const char* label, float label_width = default_label_width)
        {
            ImGui::PushID(label);
            ImGui::Columns(2, nullptr, false);
            ImGui::SetColumnWidth(0, label_width);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted(label);
            ImGui::NextColumn();
            ImGui::SetNextItemWidth(-1);
        }

        inline void end_property()
        {
            ImGui::Columns(1);
            ImGui::PopID();
        }
    }

    // --- Float ---

    inline bool property(const char* label, float& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                         const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::DragFloat("##v", &value, speed, min, max, format);
        detail::end_property();
        return changed;
    }

    // --- Int ---

    inline bool property(const char* label, int& value, float speed = 1.0f, int min = 0, int max = 0,
                         const char* format = "%d")
    {
        detail::begin_property(label);
        bool changed = ImGui::DragInt("##v", &value, speed, min, max, format);
        detail::end_property();
        return changed;
    }

    // --- Bool ---

    inline bool property(const char* label, bool& value)
    {
        detail::begin_property(label);
        bool changed = ImGui::Checkbox("##v", &value);
        detail::end_property();
        return changed;
    }

    // --- String ---

    inline bool property(const char* label, char* buffer, size_t buffer_size, ImGuiInputTextFlags flags = 0)
    {
        detail::begin_property(label);
        bool changed = ImGui::InputText("##v", buffer, buffer_size, flags);
        detail::end_property();
        return changed;
    }

    inline bool property(const char* label, std::string& value, ImGuiInputTextFlags flags = 0)
    {
        detail::begin_property(label);
        bool changed = ImGui::InputText("##v", &value, flags);
        detail::end_property();
        return changed;
    }

    // --- Vec2 ---

    inline bool property_vec2(const char* label, float v[2], float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                              const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::DragFloat2("##v", v, speed, min, max, format);
        detail::end_property();
        return changed;
    }

    // --- Vec3 ---

    inline bool property_vec3(const char* label, float v[3], float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                              const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::DragFloat3("##v", v, speed, min, max, format);
        detail::end_property();
        return changed;
    }

    // --- Vec4 ---

    inline bool property_vec4(const char* label, float v[4], float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                              const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::DragFloat4("##v", v, speed, min, max, format);
        detail::end_property();
        return changed;
    }

    // --- GLM vec2 ---

    inline bool property(const char* label, glm::vec2& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                         const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::DragFloat2("##v", glm::value_ptr(value), speed, min, max, format);
        detail::end_property();
        return changed;
    }

    // --- GLM vec3 ---

    inline bool property(const char* label, glm::vec3& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                         const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::DragFloat3("##v", glm::value_ptr(value), speed, min, max, format);
        detail::end_property();
        return changed;
    }

    // --- GLM vec4 ---

    inline bool property(const char* label, glm::vec4& value, float speed = 0.1f, float min = 0.0f, float max = 0.0f,
                         const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::DragFloat4("##v", glm::value_ptr(value), speed, min, max, format);
        detail::end_property();
        return changed;
    }

    // --- GLM color (vec3 as RGB) ---

    inline bool property_color(const char* label, glm::vec3& color, ImGuiColorEditFlags flags = 0)
    {
        detail::begin_property(label);
        bool changed = ImGui::ColorEdit3("##v", glm::value_ptr(color), flags);
        detail::end_property();
        return changed;
    }

    // --- GLM color (vec4 as RGBA) ---

    inline bool property_color(const char* label, glm::vec4& color, ImGuiColorEditFlags flags = 0)
    {
        detail::begin_property(label);
        bool changed = ImGui::ColorEdit4("##v", glm::value_ptr(color), flags);
        detail::end_property();
        return changed;
    }

    // --- GLM slider vec2 ---

    inline bool property_slider(const char* label, glm::vec2& value, float min, float max,
                                const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::SliderFloat2("##v", glm::value_ptr(value), min, max, format);
        detail::end_property();
        return changed;
    }

    // --- GLM slider vec3 ---

    inline bool property_slider(const char* label, glm::vec3& value, float min, float max,
                                const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::SliderFloat3("##v", glm::value_ptr(value), min, max, format);
        detail::end_property();
        return changed;
    }

    // --- GLM slider vec4 ---

    inline bool property_slider(const char* label, glm::vec4& value, float min, float max,
                                const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::SliderFloat4("##v", glm::value_ptr(value), min, max, format);
        detail::end_property();
        return changed;
    }

    // --- Slider ---

    inline bool property_slider(const char* label, float& value, float min, float max,
                                const char* format = "%.3f")
    {
        detail::begin_property(label);
        bool changed = ImGui::SliderFloat("##v", &value, min, max, format);
        detail::end_property();
        return changed;
    }

    inline bool property_slider(const char* label, int& value, int min, int max,
                                const char* format = "%d")
    {
        detail::begin_property(label);
        bool changed = ImGui::SliderInt("##v", &value, min, max, format);
        detail::end_property();
        return changed;
    }

    // --- Color ---

    inline bool property_color3(const char* label, float color[3], ImGuiColorEditFlags flags = 0)
    {
        detail::begin_property(label);
        bool changed = ImGui::ColorEdit3("##v", color, flags);
        detail::end_property();
        return changed;
    }

    inline bool property_color4(const char* label, float color[4], ImGuiColorEditFlags flags = 0)
    {
        detail::begin_property(label);
        bool changed = ImGui::ColorEdit4("##v", color, flags);
        detail::end_property();
        return changed;
    }

    // --- Dropdown ---

    inline bool property_dropdown(const char* label, int& current, const char* const items[], int item_count)
    {
        detail::begin_property(label);
        bool changed = ImGui::Combo("##v", &current, items, item_count);
        detail::end_property();
        return changed;
    }

    inline bool property_dropdown(const char* label, int& current, const char* items_separated_by_zeros)
    {
        detail::begin_property(label);
        bool changed = ImGui::Combo("##v", &current, items_separated_by_zeros);
        detail::end_property();
        return changed;
    }

    inline bool property_dropdown(const char* label, int& current, std::span<const char* const> items)
    {
        detail::begin_property(label);
        bool changed = ImGui::Combo("##v", &current, items.data(), static_cast<int>(items.size()));
        detail::end_property();
        return changed;
    }

    // --- Angle (degrees) ---

    inline bool property_angle(const char* label, float& radians, float min_deg = -360.0f, float max_deg = 360.0f,
                               const char* format = "%.0f deg")
    {
        detail::begin_property(label);
        bool changed = ImGui::SliderAngle("##v", &radians, min_deg, max_deg, format);
        detail::end_property();
        return changed;
    }

    // --- Custom widget ---

    template<std::invocable F>
    requires std::same_as<std::invoke_result_t<F>, bool>
    inline bool property_custom(const char* label, F&& widget_fn)
    {
        detail::begin_property(label);
        bool changed = widget_fn();
        detail::end_property();
        return changed;
    }

    // --- Read-only text property ---

    inline void property_text(const char* label, const char* value)
    {
        detail::begin_property(label);
        ImGui::TextUnformatted(value);
        detail::end_property();
    }
}
