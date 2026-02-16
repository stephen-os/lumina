#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

#include <cstdint>

namespace lumina::ui
{
    namespace utils
    {
        inline ImTextureID to_imgui_tex(void* tex) { return (ImTextureID)(uintptr_t)tex; }
        inline ImVec2 to_imvec2(const glm::vec2& v) { return ImVec2(v.x, v.y); }
        inline ImVec2 to_imvec2(float x, float y) { return ImVec2(x, y); }
        inline glm::vec2 to_glm(const ImVec2& v) { return glm::vec2(v.x, v.y); }
    }

    // Image display
    inline void image(void* tex_id, const glm::vec2& size)
    {
        ImGui::Image(utils::to_imgui_tex(tex_id), utils::to_imvec2(size));
    }

    inline void image(void* tex_id, float width, float height)
    {
        ImGui::Image(utils::to_imgui_tex(tex_id), ImVec2(width, height));
    }

    inline void image(void* tex_id, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1)
    {
        ImGui::Image(utils::to_imgui_tex(tex_id), utils::to_imvec2(size),
                     utils::to_imvec2(uv0), utils::to_imvec2(uv1));
    }

    // Image bounds returned after drawing - useful for mouse picking, coordinate mapping
    struct image_rect
    {
        glm::vec2 pos;   // Screen position (top-left)
        glm::vec2 size;  // Actual rendered size
    };

    inline image_rect image_with_rect(void* tex_id, const glm::vec2& size)
    {
        ImGui::Image(utils::to_imgui_tex(tex_id), utils::to_imvec2(size));
        return { utils::to_glm(ImGui::GetItemRectMin()), utils::to_glm(ImGui::GetItemRectSize()) };
    }

    inline image_rect image_with_rect(void* tex_id, float width, float height)
    {
        ImGui::Image(utils::to_imgui_tex(tex_id), ImVec2(width, height));
        return { utils::to_glm(ImGui::GetItemRectMin()), utils::to_glm(ImGui::GetItemRectSize()) };
    }

    inline image_rect image_with_rect(void* tex_id, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1)
    {
        ImGui::Image(utils::to_imgui_tex(tex_id), utils::to_imvec2(size),
                     utils::to_imvec2(uv0), utils::to_imvec2(uv1));
        return { utils::to_glm(ImGui::GetItemRectMin()), utils::to_glm(ImGui::GetItemRectSize()) };
    }

    // Image buttons
    inline bool image_button(const char* str_id, void* tex_id, const glm::vec2& size)
    {
        return ImGui::ImageButton(str_id, utils::to_imgui_tex(tex_id), utils::to_imvec2(size));
    }

    inline bool image_button(const char* str_id, void* tex_id, float width, float height)
    {
        return ImGui::ImageButton(str_id, utils::to_imgui_tex(tex_id), ImVec2(width, height));
    }

    inline bool image_button(const char* str_id, void* tex_id, const glm::vec2& size,
                            const glm::vec2& uv0, const glm::vec2& uv1,
                            const glm::vec4& bg_col = glm::vec4(0, 0, 0, 0),
                            const glm::vec4& tint_col = glm::vec4(1, 1, 1, 1))
    {
        return ImGui::ImageButton(str_id, utils::to_imgui_tex(tex_id), utils::to_imvec2(size),
                                  utils::to_imvec2(uv0), utils::to_imvec2(uv1),
                                  ImVec4(bg_col.r, bg_col.g, bg_col.b, bg_col.a),
                                  ImVec4(tint_col.r, tint_col.g, tint_col.b, tint_col.a));
    }
}
