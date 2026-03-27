#include "Theme.h"

#include <imgui.h>

namespace Lumina
{
	void Theme::ApplyLuminaTheme()
	{
		ImGui::StyleColorsDark();

		ImGuiStyle& s = ImGui::GetStyle();
		ImVec4* colors = s.Colors;

		// Window
		colors[ImGuiCol_WindowBg] = Color::DarkGray;
		colors[ImGuiCol_ChildBg] = Color::DarkGray;
		colors[ImGuiCol_PopupBg] = Color::MediumGray;
		colors[ImGuiCol_MenuBarBg] = Color::MediumGray;

		// Title bar
		colors[ImGuiCol_TitleBg] = Color::MediumGray;
		colors[ImGuiCol_TitleBgActive] = Color::LightGray;
		colors[ImGuiCol_TitleBgCollapsed] = Color::MediumGrayTransparent;

		// Frame (inputs, combo boxes, etc.)
		colors[ImGuiCol_FrameBg] = Color::MediumGray;
		colors[ImGuiCol_FrameBgHovered] = Color::LightGray;
		colors[ImGuiCol_FrameBgActive] = Color::ExtraLightGray;

		// Buttons
		colors[ImGuiCol_Button] = Color::MediumGray;
		colors[ImGuiCol_ButtonHovered] = Color::LightGray;
		colors[ImGuiCol_ButtonActive] = Color::ExtraLightGray;

		// Headers (tree nodes, collapsing headers, etc.)
		colors[ImGuiCol_Header] = Color::OrangeSecondary;
		colors[ImGuiCol_HeaderHovered] = Color::OrangeHover;
		colors[ImGuiCol_HeaderActive] = Color::OrangeActive;

		// Selection
		colors[ImGuiCol_CheckMark] = Color::OrangePrimary;
		colors[ImGuiCol_SliderGrab] = Color::OrangePrimary;
		colors[ImGuiCol_SliderGrabActive] = Color::OrangeLight;

		// Tabs
		colors[ImGuiCol_Tab] = Color::OrangeSecondary;
		colors[ImGuiCol_TabHovered] = Color::OrangeHover;
		colors[ImGuiCol_TabActive] = Color::OrangeActive;
		colors[ImGuiCol_TabUnfocused] = Color::DarkGray;
		colors[ImGuiCol_TabUnfocusedActive] = Color::MediumGray;

		// Text
		colors[ImGuiCol_Text] = Color::TextPrimary;
		colors[ImGuiCol_TextDisabled] = Color::TextDisabled;
		colors[ImGuiCol_TextSelectedBg] = Color::OrangeSecondary;

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg] = Color::DarkGray;
		colors[ImGuiCol_ScrollbarGrab] = Color::MediumGray;
		colors[ImGuiCol_ScrollbarGrabHovered] = Color::LightGray;
		colors[ImGuiCol_ScrollbarGrabActive] = Color::OrangePrimary;

		// Separator
		colors[ImGuiCol_Separator] = Color::MediumGray;
		colors[ImGuiCol_SeparatorHovered] = Color::OrangeHover;
		colors[ImGuiCol_SeparatorActive] = Color::OrangeActive;

		// Resize grip
		colors[ImGuiCol_ResizeGrip] = Color::MediumGray;
		colors[ImGuiCol_ResizeGripHovered] = Color::OrangeHover;
		colors[ImGuiCol_ResizeGripActive] = Color::OrangeActive;

		// Docking
		colors[ImGuiCol_DockingPreview] = Color::OrangeSecondary;
		colors[ImGuiCol_DockingEmptyBg] = Color::DarkGray;

		// Tables
		colors[ImGuiCol_TableHeaderBg] = Color::MediumGray;
		colors[ImGuiCol_TableBorderStrong] = Color::LightGray;
		colors[ImGuiCol_TableBorderLight] = Color::MediumGray;
		colors[ImGuiCol_TableRowBg] = Color::Transparent;
		colors[ImGuiCol_TableRowBgAlt] = Color::SubtleWhite;

		// Navigation
		colors[ImGuiCol_NavHighlight] = Color::OrangePrimary;
		colors[ImGuiCol_NavWindowingHighlight] = Color::OrangeActive;
		colors[ImGuiCol_NavWindowingDimBg] = Color::NavDim;

		// Modal
		colors[ImGuiCol_ModalWindowDimBg] = Color::ModalDim;

		// Plots
		colors[ImGuiCol_PlotLines] = Color::OrangePrimary;
		colors[ImGuiCol_PlotLinesHovered] = Color::OrangeLight;
		colors[ImGuiCol_PlotHistogram] = Color::OrangeSecondary;
		colors[ImGuiCol_PlotHistogramHovered] = Color::OrangeActive;

		// Borders
		colors[ImGuiCol_Border] = Color::MediumGray;
		colors[ImGuiCol_BorderShadow] = Color::Transparent;

		// Style settings
		s.WindowRounding = Style::WindowRounding;
		s.WindowPadding = Style::WindowPadding;
		s.FrameRounding = Style::FrameRounding;
		s.FramePadding = Style::FramePadding;
		s.ItemSpacing = Style::ItemSpacing;
		s.ItemInnerSpacing = Style::ItemInnerSpacing;
		s.IndentSpacing = Style::IndentSpacing;
		s.ScrollbarSize = Style::ScrollbarSize;
		s.ScrollbarRounding = Style::ScrollbarRounding;
		s.GrabMinSize = Style::GrabMinSize;
		s.GrabRounding = Style::GrabRounding;
		s.TabRounding = Style::TabRounding;
		s.TabBorderSize = Style::TabBorderSize;
		s.FrameBorderSize = Style::FrameBorderSize;

		// Viewport compatibility
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			s.WindowRounding = 0.0f;
			colors[ImGuiCol_WindowBg].w = 1.0f;
		}
	}
}
