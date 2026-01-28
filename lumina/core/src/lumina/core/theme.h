#pragma once

#include <imgui.h>

namespace lumina::core
{
    namespace color
    {
        // Base colors
        inline const ImVec4 dark_gray = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        inline const ImVec4 medium_gray = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        inline const ImVec4 light_gray = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        inline const ImVec4 extra_light_gray = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

        // Orange accent colors
        inline const ImVec4 orange_primary = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
        inline const ImVec4 orange_secondary = ImVec4(0.8f, 0.4f, 0.1f, 0.8f);
        inline const ImVec4 orange_hover = ImVec4(0.9f, 0.5f, 0.2f, 0.8f);
        inline const ImVec4 orange_active = ImVec4(1.0f, 0.6f, 0.3f, 1.0f);
        inline const ImVec4 orange_light = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);

        // Text colors
        inline const ImVec4 text_primary = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        inline const ImVec4 text_secondary = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        inline const ImVec4 text_disabled = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

        // Status colors
        inline const ImVec4 success = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
        inline const ImVec4 warning = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
        inline const ImVec4 error = ImVec4(0.9f, 0.2f, 0.2f, 1.0f);

        // Transparency variants
        inline const ImVec4 medium_gray_transparent = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);
        inline const ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        inline const ImVec4 subtle_white = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
        inline const ImVec4 modal_dim = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
        inline const ImVec4 nav_dim = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
    }

    namespace style
    {
        inline const float window_rounding = 5.0f;
        inline const ImVec2 window_padding = ImVec2(8.0f, 8.0f);
        inline const float frame_rounding = 5.0f;
        inline const ImVec2 frame_padding = ImVec2(5.0f, 5.0f);
        inline const ImVec2 item_spacing = ImVec2(8.0f, 6.0f);
        inline const ImVec2 item_inner_spacing = ImVec2(4.0f, 4.0f);
        inline const float indent_spacing = 21.0f;
        inline const float scrollbar_size = 14.0f;
        inline const float scrollbar_rounding = 9.0f;
        inline const float grab_min_size = 10.0f;
        inline const float grab_rounding = 3.0f;
        inline const float tab_rounding = 4.0f;
        inline const float tab_border_size = 0.0f;
        inline const float frame_border_size = 0.0f;
    }

    class theme
    {
    public:
        static void apply_lumina_theme();
    };
}
