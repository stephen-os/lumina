#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

#include <concepts>

namespace lumina::ui
{
    inline void separator() { ImGui::Separator(); }
    inline void spacing() { ImGui::Spacing(); }
    inline void same_line(float offset = 0.0f, float spacing = -1.0f) { ImGui::SameLine(offset, spacing); }
    inline void indent(float width = 0.0f) { ImGui::Indent(width); }
    inline void unindent(float width = 0.0f) { ImGui::Unindent(width); }
    inline void new_line() { ImGui::NewLine(); }
    inline void dummy(const ImVec2& size) { ImGui::Dummy(size); }
    inline void dummy(const glm::vec2& size) { ImGui::Dummy(ImVec2(size.x, size.y)); }

    inline float get_content_width() { return ImGui::GetContentRegionAvail().x; }
    inline float get_content_height() { return ImGui::GetContentRegionAvail().y; }
    inline ImVec2 get_content_size() { return ImGui::GetContentRegionAvail(); }

    inline void indented(float width, std::invocable auto&& body)
    {
        ImGui::Indent(width);
        body();
        ImGui::Unindent(width);
    }

    inline void indented(std::invocable auto&& body)
    {
        ImGui::Indent();
        body();
        ImGui::Unindent();
    }

    inline void with_width(float width, std::invocable auto&& body)
    {
        ImGui::PushItemWidth(width);
        body();
        ImGui::PopItemWidth();
    }

    inline void with_id(const char* id, std::invocable auto&& body)
    {
        ImGui::PushID(id);
        body();
        ImGui::PopID();
    }

    inline void with_id(int id, std::invocable auto&& body)
    {
        ImGui::PushID(id);
        body();
        ImGui::PopID();
    }

    inline void with_id(const void* ptr, std::invocable auto&& body)
    {
        ImGui::PushID(ptr);
        body();
        ImGui::PopID();
    }
}
