#pragma once

#include <imgui.h>

#include <cstdint>

namespace lumina::ui
{
    namespace utils
    {
        inline ImTextureID to_imgui_tex(void* tex) { return (ImTextureID)(uintptr_t)tex; }
    }

    inline void image(void* tex_id, const ImVec2& size)
    {
        ImGui::Image(utils::to_imgui_tex(tex_id), size);
    }

    inline void image(void* tex_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1)
    {
        ImGui::Image(utils::to_imgui_tex(tex_id), size, uv0, uv1);
    }

    // Image bounds returned after drawing - useful for mouse picking, coordinate mapping
    struct image_rect
    {
        ImVec2 pos;   // Screen position (top-left)
        ImVec2 size;  // Actual rendered size
    };

    inline image_rect image_with_rect(void* tex_id, const ImVec2& size)
    {
        ImGui::Image(utils::to_imgui_tex(tex_id), size);
        return { ImGui::GetItemRectMin(), ImGui::GetItemRectSize() };
    }

    inline image_rect image_with_rect(void* tex_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1)
    {
        ImGui::Image(utils::to_imgui_tex(tex_id), size, uv0, uv1);
        return { ImGui::GetItemRectMin(), ImGui::GetItemRectSize() };
    }

    inline bool image_button(const char* str_id, void* tex_id, const ImVec2& size)
    {
        return ImGui::ImageButton(str_id, utils::to_imgui_tex(tex_id), size);
    }

    inline bool image_button(const char* str_id, void* tex_id, const ImVec2& size,
                            const ImVec2& uv0, const ImVec2& uv1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0),
                            const ImVec4& tint_col = ImVec4(1, 1, 1, 1))
    {
        return ImGui::ImageButton(str_id, utils::to_imgui_tex(tex_id), size, uv0, uv1, bg_col, tint_col);
    }
}
