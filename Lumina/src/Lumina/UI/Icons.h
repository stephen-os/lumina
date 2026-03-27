#pragma once

#include <imgui.h>

#include <cstdio>

namespace Lumina::UI::Icons
{
    // Placeholder icon constants.
    // Replace with actual UTF-8 codepoints when an icon font (FontAwesome, MaterialIcons, etc.) is integrated.
    inline constexpr const char* Folder      = "F";
    inline constexpr const char* File        = "f";
    inline constexpr const char* Save        = "S";
    inline constexpr const char* Open        = "O";
    inline constexpr const char* Close       = "X";
    inline constexpr const char* Play        = ">";
    inline constexpr const char* Pause       = "||";
    inline constexpr const char* Stop        = "#";
    inline constexpr const char* Add         = "+";
    inline constexpr const char* Remove      = "-";
    inline constexpr const char* Edit        = "E";
    inline constexpr const char* Search      = "?";
    inline constexpr const char* Settings    = "*";
    inline constexpr const char* Refresh     = "R";
    inline constexpr const char* Trash       = "D";
    inline constexpr const char* Eye         = "V";
    inline constexpr const char* EyeOff      = "H";
    inline constexpr const char* Lock        = "L";
    inline constexpr const char* Unlock      = "U";
    inline constexpr const char* ArrowUp     = "^";
    inline constexpr const char* ArrowDown   = "v";
    inline constexpr const char* ArrowLeft   = "<";
    inline constexpr const char* ArrowRight  = ">";
}

namespace Lumina::UI
{
    [[nodiscard]] inline bool IconButton(const char* icon, const char* label, const ImVec2& size = {0, 0})
    {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "%s %s", icon, label);
        return ImGui::Button(buffer, size);
    }

    [[nodiscard]] inline bool IconButton(const char* icon, const ImVec2& size = {0, 0})
    {
        return ImGui::Button(icon, size);
    }
}
