#pragma once

#include <imgui.h>
#include <glm/glm.hpp>

#include <cstdint>

namespace Lumina::UI
{
    namespace Utils
    {
        inline ImTextureID ToImGuiTex(void* tex) { return (ImTextureID)(uintptr_t)tex; }
        inline ImVec2 ToImVec2(const glm::vec2& v) { return ImVec2(v.x, v.y); }
        inline ImVec2 ToImVec2(float x, float y) { return ImVec2(x, y); }
        inline glm::vec2 ToGlm(const ImVec2& v) { return glm::vec2(v.x, v.y); }
    }

    // Image display
    inline void Image(void* texId, const glm::vec2& size)
    {
        ImGui::Image(Utils::ToImGuiTex(texId), Utils::ToImVec2(size));
    }

    inline void Image(void* texId, float width, float height)
    {
        ImGui::Image(Utils::ToImGuiTex(texId), ImVec2(width, height));
    }

    inline void Image(void* texId, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1)
    {
        ImGui::Image(Utils::ToImGuiTex(texId), Utils::ToImVec2(size),
                     Utils::ToImVec2(uv0), Utils::ToImVec2(uv1));
    }

    // Image bounds returned after drawing - useful for mouse picking, coordinate mapping
    struct ImageRect
    {
        glm::vec2 Pos;   // Screen position (top-left)
        glm::vec2 Size;  // Actual rendered size
    };

    inline ImageRect ImageWithRect(void* texId, const glm::vec2& size)
    {
        ImGui::Image(Utils::ToImGuiTex(texId), Utils::ToImVec2(size));
        return { Utils::ToGlm(ImGui::GetItemRectMin()), Utils::ToGlm(ImGui::GetItemRectSize()) };
    }

    inline ImageRect ImageWithRect(void* texId, float width, float height)
    {
        ImGui::Image(Utils::ToImGuiTex(texId), ImVec2(width, height));
        return { Utils::ToGlm(ImGui::GetItemRectMin()), Utils::ToGlm(ImGui::GetItemRectSize()) };
    }

    inline ImageRect ImageWithRect(void* texId, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1)
    {
        ImGui::Image(Utils::ToImGuiTex(texId), Utils::ToImVec2(size),
                     Utils::ToImVec2(uv0), Utils::ToImVec2(uv1));
        return { Utils::ToGlm(ImGui::GetItemRectMin()), Utils::ToGlm(ImGui::GetItemRectSize()) };
    }

    // Image buttons
    inline bool ImageButton(const char* strId, void* texId, const glm::vec2& size)
    {
        return ImGui::ImageButton(strId, Utils::ToImGuiTex(texId), Utils::ToImVec2(size));
    }

    inline bool ImageButton(const char* strId, void* texId, float width, float height)
    {
        return ImGui::ImageButton(strId, Utils::ToImGuiTex(texId), ImVec2(width, height));
    }

    inline bool ImageButton(const char* strId, void* texId, const glm::vec2& size,
                            const glm::vec2& uv0, const glm::vec2& uv1,
                            const glm::vec4& bgCol = glm::vec4(0, 0, 0, 0),
                            const glm::vec4& tintCol = glm::vec4(1, 1, 1, 1))
    {
        return ImGui::ImageButton(strId, Utils::ToImGuiTex(texId), Utils::ToImVec2(size),
                                  Utils::ToImVec2(uv0), Utils::ToImVec2(uv1),
                                  ImVec4(bgCol.r, bgCol.g, bgCol.b, bgCol.a),
                                  ImVec4(tintCol.r, tintCol.g, tintCol.b, tintCol.a));
    }
}
