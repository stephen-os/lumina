#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

namespace lumina::ui
{
    // Layout utilities
    inline void separator() { ImGui::Separator(); }
    inline void spacing() { ImGui::Spacing(); }
    inline void same_line(float offset = 0.0f, float spacing = -1.0f) { ImGui::SameLine(offset, spacing); }
    inline void new_line() { ImGui::NewLine(); }
    inline void dummy(const ImVec2& size) { ImGui::Dummy(size); }
    inline void dummy(const glm::vec2& size) { ImGui::Dummy(ImVec2(size.x, size.y)); }

    // Indent
    inline void indent(float width = 0.0f) { ImGui::Indent(width); }
    inline void unindent(float width = 0.0f) { ImGui::Unindent(width); }

    // Item width
    inline void push_item_width(float width) { ImGui::PushItemWidth(width); }
    inline void pop_item_width() { ImGui::PopItemWidth(); }
    inline void set_next_item_width(float width) { ImGui::SetNextItemWidth(width); }

    // ID stack
    inline void push_id(const char* id) { ImGui::PushID(id); }
    inline void push_id(int id) { ImGui::PushID(id); }
    inline void push_id(const void* ptr) { ImGui::PushID(ptr); }
    inline void pop_id() { ImGui::PopID(); }

    // Content region queries
    inline float get_content_width() { return ImGui::GetContentRegionAvail().x; }
    inline float get_content_height() { return ImGui::GetContentRegionAvail().y; }
    inline ImVec2 get_content_size() { return ImGui::GetContentRegionAvail(); }

    // Note: Cursor positioning functions are in state.h
}
