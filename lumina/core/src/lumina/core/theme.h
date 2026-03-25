#pragma once

#include <imgui.h>

namespace Lumina
{
	namespace Color
	{
		// Base colors
		inline const ImVec4 DarkGray = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
		inline const ImVec4 MediumGray = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		inline const ImVec4 LightGray = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		inline const ImVec4 ExtraLightGray = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

		// Orange accent colors
		inline const ImVec4 OrangePrimary = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
		inline const ImVec4 OrangeSecondary = ImVec4(0.8f, 0.4f, 0.1f, 0.8f);
		inline const ImVec4 OrangeHover = ImVec4(0.9f, 0.5f, 0.2f, 0.8f);
		inline const ImVec4 OrangeActive = ImVec4(1.0f, 0.6f, 0.3f, 1.0f);
		inline const ImVec4 OrangeLight = ImVec4(1.0f, 0.6f, 0.2f, 1.0f);

		// Text colors
		inline const ImVec4 TextPrimary = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		inline const ImVec4 TextSecondary = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
		inline const ImVec4 TextDisabled = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

		// Status colors
		inline const ImVec4 Success = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
		inline const ImVec4 Warning = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
		inline const ImVec4 Error = ImVec4(0.9f, 0.2f, 0.2f, 1.0f);

		// Transparency variants
		inline const ImVec4 MediumGrayTransparent = ImVec4(0.2f, 0.2f, 0.2f, 0.6f);
		inline const ImVec4 Transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		inline const ImVec4 SubtleWhite = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
		inline const ImVec4 ModalDim = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
		inline const ImVec4 NavDim = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
	}

	namespace Style
	{
		inline const float WindowRounding = 5.0f;
		inline const ImVec2 WindowPadding = ImVec2(8.0f, 8.0f);
		inline const float FrameRounding = 5.0f;
		inline const ImVec2 FramePadding = ImVec2(5.0f, 5.0f);
		inline const ImVec2 ItemSpacing = ImVec2(8.0f, 6.0f);
		inline const ImVec2 ItemInnerSpacing = ImVec2(4.0f, 4.0f);
		inline const float IndentSpacing = 21.0f;
		inline const float ScrollbarSize = 14.0f;
		inline const float ScrollbarRounding = 9.0f;
		inline const float GrabMinSize = 10.0f;
		inline const float GrabRounding = 3.0f;
		inline const float TabRounding = 4.0f;
		inline const float TabBorderSize = 0.0f;
		inline const float FrameBorderSize = 0.0f;
	}

	class Theme
	{
	public:
		static void ApplyLuminaTheme();
	};
}
