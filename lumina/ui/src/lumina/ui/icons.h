#pragma once

#include <imgui.h>

#include <string>

namespace lumina::ui::icons
{
    // Placeholder icon constants.
    // Replace with actual UTF-8 codepoints when an icon font (FontAwesome, MaterialIcons, etc.) is integrated.
    inline constexpr const char* folder      = "F";
    inline constexpr const char* file        = "f";
    inline constexpr const char* save        = "S";
    inline constexpr const char* open        = "O";
    inline constexpr const char* close       = "X";
    inline constexpr const char* play        = ">";
    inline constexpr const char* pause       = "||";
    inline constexpr const char* stop        = "#";
    inline constexpr const char* add         = "+";
    inline constexpr const char* remove      = "-";
    inline constexpr const char* edit        = "E";
    inline constexpr const char* search      = "?";
    inline constexpr const char* settings    = "*";
    inline constexpr const char* refresh     = "R";
    inline constexpr const char* trash       = "D";
    inline constexpr const char* eye         = "V";
    inline constexpr const char* eye_off     = "H";
    inline constexpr const char* lock        = "L";
    inline constexpr const char* unlock      = "U";
    inline constexpr const char* arrow_up    = "^";
    inline constexpr const char* arrow_down  = "v";
    inline constexpr const char* arrow_left  = "<";
    inline constexpr const char* arrow_right = ">";
}

namespace lumina::ui
{
    inline bool icon_button(const char* icon, const char* label, const ImVec2& size = {0, 0})
    {
        auto text = std::string(icon) + " " + label;
        return ImGui::Button(text.c_str(), size);
    }

    inline bool icon_button(const char* icon, const ImVec2& size = {0, 0})
    {
        return ImGui::Button(icon, size);
    }
}
