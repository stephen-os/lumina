#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

namespace Lumina::UI
{
    // Layout utilities
    inline void Separator() { ImGui::Separator(); }
    inline void Spacing() { ImGui::Spacing(); }
    inline void SameLine(float offset = 0.0f, float spacing = -1.0f) { ImGui::SameLine(offset, spacing); }
    inline void NewLine() { ImGui::NewLine(); }
    inline void Dummy(const glm::vec2& size) { ImGui::Dummy(ImVec2(size.x, size.y)); }
    inline void Dummy(float width, float height) { ImGui::Dummy(ImVec2(width, height)); }

    // Indent
    inline void Indent(float width = 0.0f) { ImGui::Indent(width); }
    inline void Unindent(float width = 0.0f) { ImGui::Unindent(width); }

    // Item width
    inline void PushItemWidth(float width) { ImGui::PushItemWidth(width); }
    inline void PopItemWidth() { ImGui::PopItemWidth(); }
    inline void SetNextItemWidth(float width) { ImGui::SetNextItemWidth(width); }

    // ID stack
    inline void PushID(const char* id) { ImGui::PushID(id); }
    inline void PushID(int id) { ImGui::PushID(id); }
    inline void PushID(const void* ptr) { ImGui::PushID(ptr); }
    inline void PopID() { ImGui::PopID(); }

    // Content region queries
    [[nodiscard]] inline float GetContentWidth() { return ImGui::GetContentRegionAvail().x; }
    [[nodiscard]] inline float GetContentHeight() { return ImGui::GetContentRegionAvail().y; }
    [[nodiscard]] inline glm::vec2 GetContentSize() { ImVec2 size = ImGui::GetContentRegionAvail(); return { size.x, size.y }; }
}
