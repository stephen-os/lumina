#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

#include <format>
#include <string>

namespace lumina::ui
{
    inline void text(const char* t)
    {
        ImGui::TextUnformatted(t);
    }

    inline void text(const std::string& t)
    {
        ImGui::TextUnformatted(t.data(), t.data() + t.size());
    }

    inline void text_colored(const ImVec4& color, const char* t)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(t);
        ImGui::PopStyleColor();
    }

    inline void text_colored(const glm::vec4& color, const char* t)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, color.a));
        ImGui::TextUnformatted(t);
        ImGui::PopStyleColor();
    }

    inline void text_colored(const glm::vec3& color, const char* t)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color.r, color.g, color.b, 1.0f));
        ImGui::TextUnformatted(t);
        ImGui::PopStyleColor();
    }

    inline void text_disabled(const char* t)
    {
        ImGui::TextDisabled("%s", t);
    }

    inline void text_wrapped(const char* t)
    {
        ImGui::TextWrapped("%s", t);
    }

    inline void text_bullet(const char* t)
    {
        ImGui::BulletText("%s", t);
    }

    template<typename... Args>
    inline void text_fmt(std::format_string<Args...> fmt, Args&&... args)
    {
        auto s = std::format(fmt, std::forward<Args>(args)...);
        ImGui::TextUnformatted(s.data(), s.data() + s.size());
    }

    template<typename... Args>
    inline void text_colored_fmt(const ImVec4& color, std::format_string<Args...> fmt, Args&&... args)
    {
        auto s = std::format(fmt, std::forward<Args>(args)...);
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextUnformatted(s.data(), s.data() + s.size());
        ImGui::PopStyleColor();
    }

    inline void text_label(const char* label, const char* value)
    {
        ImGui::TextUnformatted(label);
        ImGui::SameLine();
        ImGui::TextDisabled("%s", value);
    }
}
