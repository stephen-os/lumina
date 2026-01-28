#include "theme.h"

#include <imgui.h>

namespace lumina::core
{
    namespace color
    {
        // Base colors
        const ImVec4 dark_gray = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        const ImVec4 medium_gray = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        const ImVec4 light_gray = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        const ImVec4 extra_light_gray = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

        // Orange accent colors
        const ImVec4 orange_primary = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
        const ImVec4 orange_secondary = ImVec4(0.8f, 0.4f, 0.1f, 0.8f);
        const ImVec4 orange_hover = ImVec4(0.9f, 0.5f, 0.2f, 0.8f);
        const ImVec4 orange_active = ImVec4(1.0f, 0.6f, 0.3f, 1.0f);
        const ImVec4 orange_light = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);

        // Text colors
        const ImVec4 text_primary = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        const ImVec4 text_secondary = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        const ImVec4 text_disabled = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

        // Transparency variants
        const ImVec4 medium_gray_transparent = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);
        const ImVec4 transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        const ImVec4 subtle_white = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
        const ImVec4 modal_dim = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
        const ImVec4 nav_dim = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
    }

    namespace style
    {
        const float window_rounding = 5.0f;
        const ImVec2 window_padding = ImVec2(8.0f, 8.0f);
        const float frame_rounding = 5.0f;
        const ImVec2 frame_padding = ImVec2(5.0f, 5.0f);
        const ImVec2 item_spacing = ImVec2(8.0f, 6.0f);
        const ImVec2 item_inner_spacing = ImVec2(4.0f, 4.0f);
        const float indent_spacing = 21.0f;
        const float scrollbar_size = 14.0f;
        const float scrollbar_rounding = 9.0f;
        const float grab_min_size = 10.0f;
        const float grab_rounding = 3.0f;
        const float tab_rounding = 4.0f;
        const float tab_border_size = 0.0f;
        const float frame_border_size = 0.0f;
    }

    void theme::apply_lumina_theme()
    {
        ImGui::StyleColorsDark();

        ImGuiStyle& s = ImGui::GetStyle();
        ImVec4* colors = s.Colors;

        // Window
        colors[ImGuiCol_WindowBg] = color::dark_gray;
        colors[ImGuiCol_ChildBg] = color::dark_gray;
        colors[ImGuiCol_PopupBg] = color::medium_gray;
        colors[ImGuiCol_MenuBarBg] = color::medium_gray;

        // Title bar
        colors[ImGuiCol_TitleBg] = color::medium_gray;
        colors[ImGuiCol_TitleBgActive] = color::light_gray;
        colors[ImGuiCol_TitleBgCollapsed] = color::medium_gray_transparent;

        // Frame (inputs, combo boxes, etc.)
        colors[ImGuiCol_FrameBg] = color::medium_gray;
        colors[ImGuiCol_FrameBgHovered] = color::light_gray;
        colors[ImGuiCol_FrameBgActive] = color::extra_light_gray;

        // Buttons
        colors[ImGuiCol_Button] = color::medium_gray;
        colors[ImGuiCol_ButtonHovered] = color::light_gray;
        colors[ImGuiCol_ButtonActive] = color::extra_light_gray;

        // Headers (tree nodes, collapsing headers, etc.)
        colors[ImGuiCol_Header] = color::orange_secondary;
        colors[ImGuiCol_HeaderHovered] = color::orange_hover;
        colors[ImGuiCol_HeaderActive] = color::orange_active;

        // Selection
        colors[ImGuiCol_CheckMark] = color::orange_primary;
        colors[ImGuiCol_SliderGrab] = color::orange_primary;
        colors[ImGuiCol_SliderGrabActive] = color::orange_light;

        // Tabs
        colors[ImGuiCol_Tab] = color::orange_secondary;
        colors[ImGuiCol_TabHovered] = color::orange_hover;
        colors[ImGuiCol_TabActive] = color::orange_active;
        colors[ImGuiCol_TabUnfocused] = color::dark_gray;
        colors[ImGuiCol_TabUnfocusedActive] = color::medium_gray;

        // Text
        colors[ImGuiCol_Text] = color::text_primary;
        colors[ImGuiCol_TextDisabled] = color::text_disabled;
        colors[ImGuiCol_TextSelectedBg] = color::orange_secondary;

        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = color::dark_gray;
        colors[ImGuiCol_ScrollbarGrab] = color::medium_gray;
        colors[ImGuiCol_ScrollbarGrabHovered] = color::light_gray;
        colors[ImGuiCol_ScrollbarGrabActive] = color::orange_primary;

        // Separator
        colors[ImGuiCol_Separator] = color::medium_gray;
        colors[ImGuiCol_SeparatorHovered] = color::orange_hover;
        colors[ImGuiCol_SeparatorActive] = color::orange_active;

        // Resize grip
        colors[ImGuiCol_ResizeGrip] = color::medium_gray;
        colors[ImGuiCol_ResizeGripHovered] = color::orange_hover;
        colors[ImGuiCol_ResizeGripActive] = color::orange_active;

        // Docking
        colors[ImGuiCol_DockingPreview] = color::orange_secondary;
        colors[ImGuiCol_DockingEmptyBg] = color::dark_gray;

        // Tables
        colors[ImGuiCol_TableHeaderBg] = color::medium_gray;
        colors[ImGuiCol_TableBorderStrong] = color::light_gray;
        colors[ImGuiCol_TableBorderLight] = color::medium_gray;
        colors[ImGuiCol_TableRowBg] = color::transparent;
        colors[ImGuiCol_TableRowBgAlt] = color::subtle_white;

        // Navigation
        colors[ImGuiCol_NavHighlight] = color::orange_primary;
        colors[ImGuiCol_NavWindowingHighlight] = color::orange_active;
        colors[ImGuiCol_NavWindowingDimBg] = color::nav_dim;

        // Modal
        colors[ImGuiCol_ModalWindowDimBg] = color::modal_dim;

        // Plots
        colors[ImGuiCol_PlotLines] = color::orange_primary;
        colors[ImGuiCol_PlotLinesHovered] = color::orange_light;
        colors[ImGuiCol_PlotHistogram] = color::orange_secondary;
        colors[ImGuiCol_PlotHistogramHovered] = color::orange_active;

        // Borders
        colors[ImGuiCol_Border] = color::medium_gray;
        colors[ImGuiCol_BorderShadow] = color::transparent;

        // Style settings
        s.WindowRounding = style::window_rounding;
        s.WindowPadding = style::window_padding;
        s.FrameRounding = style::frame_rounding;
        s.FramePadding = style::frame_padding;
        s.ItemSpacing = style::item_spacing;
        s.ItemInnerSpacing = style::item_inner_spacing;
        s.IndentSpacing = style::indent_spacing;
        s.ScrollbarSize = style::scrollbar_size;
        s.ScrollbarRounding = style::scrollbar_rounding;
        s.GrabMinSize = style::grab_min_size;
        s.GrabRounding = style::grab_rounding;
        s.TabRounding = style::tab_rounding;
        s.TabBorderSize = style::tab_border_size;
        s.FrameBorderSize = style::frame_border_size;

        // Viewport compatibility
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            s.WindowRounding = 0.0f;
            colors[ImGuiCol_WindowBg].w = 1.0f;
        }
    }
}
