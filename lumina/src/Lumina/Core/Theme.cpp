#include "Theme.h"

#include <imgui.h>

namespace Lumina
{
    namespace Color
    {
        // Base colors
        const ImVec4 DarkGray = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
        const ImVec4 MediumGray = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        const ImVec4 LightGray = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        const ImVec4 ExtraLightGray = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

        // Orange accent colors
        const ImVec4 OrangePrimary = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
        const ImVec4 OrangeSecondary = ImVec4(0.8f, 0.4f, 0.1f, 0.8f);
        const ImVec4 OrangeHover = ImVec4(0.9f, 0.5f, 0.2f, 0.8f);
        const ImVec4 OrangeActive = ImVec4(1.0f, 0.6f, 0.3f, 1.0f);
        const ImVec4 OrangeLight = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);

        // Text colors
        const ImVec4 TextPrimary = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        const ImVec4 TextSecondary = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        const ImVec4 TextDisabled = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

        // Transparency variants
        const ImVec4 MediumGrayTransparent = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);
        const ImVec4 Transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        const ImVec4 SubtleWhite = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
        const ImVec4 ModalDim = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
        const ImVec4 NavDim = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
    }

    namespace Style
    {
        const float WindowRounding = 5.0f;
        const ImVec2 WindowPadding = ImVec2(8.0f, 8.0f);
        const float FrameRounding = 5.0f;
        const ImVec2 FramePadding = ImVec2(5.0f, 5.0f);
        const ImVec2 ItemSpacing = ImVec2(8.0f, 6.0f);
        const ImVec2 ItemInnerSpacing = ImVec2(4.0f, 4.0f);
        const float IndentSpacing = 21.0f;
        const float ScrollbarSize = 14.0f;
        const float ScrollbarRounding = 9.0f;
        const float GrabMinSize = 10.0f;
        const float GrabRounding = 3.0f;
        const float TabRounding = 4.0f;
        const float TabBorderSize = 0.0f;
        const float FrameBorderSize = 0.0f;
    }

    void Theme::ApplyLuminaTheme()
    {
        ImGui::StyleColorsDark(); // Start with ImGui's dark theme as base

        ImGuiStyle& style = ImGui::GetStyle();
        ImVec4* colors = style.Colors;

        // Window colors
        colors[ImGuiCol_WindowBg] = Color::DarkGray;
        colors[ImGuiCol_ChildBg] = Color::DarkGray;
        colors[ImGuiCol_PopupBg] = Color::MediumGray;
        colors[ImGuiCol_MenuBarBg] = Color::MediumGray;

        // Title bar colors
        colors[ImGuiCol_TitleBg] = Color::MediumGray;
        colors[ImGuiCol_TitleBgActive] = Color::LightGray;
        colors[ImGuiCol_TitleBgCollapsed] = Color::MediumGrayTransparent;

        // Frame colors (inputs, combo boxes, etc.)
        colors[ImGuiCol_FrameBg] = Color::MediumGray;
        colors[ImGuiCol_FrameBgHovered] = Color::LightGray;
        colors[ImGuiCol_FrameBgActive] = Color::ExtraLightGray;

        // Button colors
        colors[ImGuiCol_Button] = Color::MediumGray;
        colors[ImGuiCol_ButtonHovered] = Color::LightGray;
        colors[ImGuiCol_ButtonActive] = Color::ExtraLightGray;

        // Header colors (tree nodes, collapsing headers, etc.)
        colors[ImGuiCol_Header] = Color::OrangeSecondary;
        colors[ImGuiCol_HeaderHovered] = Color::OrangeHover;
        colors[ImGuiCol_HeaderActive] = Color::OrangeActive;

        // Selection colors
        colors[ImGuiCol_CheckMark] = Color::OrangePrimary;
        colors[ImGuiCol_SliderGrab] = Color::OrangePrimary;
        colors[ImGuiCol_SliderGrabActive] = Color::OrangeLight;

        // Tab colors
        colors[ImGuiCol_Tab] = Color::OrangeSecondary;
        colors[ImGuiCol_TabHovered] = Color::OrangeHover;
        colors[ImGuiCol_TabActive] = Color::OrangeActive;
        colors[ImGuiCol_TabUnfocused] = Color::DarkGray;
        colors[ImGuiCol_TabUnfocusedActive] = Color::MediumGray;

        // Text colors
        colors[ImGuiCol_Text] = Color::TextPrimary;
        colors[ImGuiCol_TextDisabled] = Color::TextDisabled;
        colors[ImGuiCol_TextSelectedBg] = Color::OrangeSecondary;

        // Scrollbar colors
        colors[ImGuiCol_ScrollbarBg] = Color::DarkGray;
        colors[ImGuiCol_ScrollbarGrab] = Color::MediumGray;
        colors[ImGuiCol_ScrollbarGrabHovered] = Color::LightGray;
        colors[ImGuiCol_ScrollbarGrabActive] = Color::OrangePrimary;

        // Separator colors
        colors[ImGuiCol_Separator] = Color::MediumGray;
        colors[ImGuiCol_SeparatorHovered] = Color::OrangeHover;
        colors[ImGuiCol_SeparatorActive] = Color::OrangeActive;

        // Resize grip colors
        colors[ImGuiCol_ResizeGrip] = Color::MediumGray;
        colors[ImGuiCol_ResizeGripHovered] = Color::OrangeHover;
        colors[ImGuiCol_ResizeGripActive] = Color::OrangeActive;

        // Docking colors
        colors[ImGuiCol_DockingPreview] = Color::OrangeSecondary;
        colors[ImGuiCol_DockingEmptyBg] = Color::DarkGray;

        // Table colors
        colors[ImGuiCol_TableHeaderBg] = Color::MediumGray;
        colors[ImGuiCol_TableBorderStrong] = Color::LightGray;
        colors[ImGuiCol_TableBorderLight] = Color::MediumGray;
        colors[ImGuiCol_TableRowBg] = Color::Transparent;
        colors[ImGuiCol_TableRowBgAlt] = Color::SubtleWhite;

        // Navigation colors
        colors[ImGuiCol_NavHighlight] = Color::OrangePrimary;
        colors[ImGuiCol_NavWindowingHighlight] = Color::OrangeActive;
        colors[ImGuiCol_NavWindowingDimBg] = Color::NavDim;

        // Modal colors
        colors[ImGuiCol_ModalWindowDimBg] = Color::ModalDim;

        // Plot colors
        colors[ImGuiCol_PlotLines] = Color::OrangePrimary;
        colors[ImGuiCol_PlotLinesHovered] = Color::OrangeLight;
        colors[ImGuiCol_PlotHistogram] = Color::OrangeSecondary;
        colors[ImGuiCol_PlotHistogramHovered] = Color::OrangeActive;

        // Border colors
        colors[ImGuiCol_Border] = Color::MediumGray;
        colors[ImGuiCol_BorderShadow] = Color::Transparent;

        // Apply spacing and style settings
        style.WindowRounding = Style::WindowRounding;
        style.WindowPadding = Style::WindowPadding;
        style.FrameRounding = Style::FrameRounding;
        style.FramePadding = Style::FramePadding;
        style.ItemSpacing = Style::ItemSpacing;
        style.ItemInnerSpacing = Style::ItemInnerSpacing;
        style.IndentSpacing = Style::IndentSpacing;
        style.ScrollbarSize = Style::ScrollbarSize;
        style.ScrollbarRounding = Style::ScrollbarRounding;
        style.GrabMinSize = Style::GrabMinSize;
        style.GrabRounding = Style::GrabRounding;
        style.TabRounding = Style::TabRounding;
        style.TabBorderSize = Style::TabBorderSize;
        style.FrameBorderSize = Style::FrameBorderSize;

        // Handle viewport compatibility
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            colors[ImGuiCol_WindowBg].w = 1.0f;
        }
    }
}