#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace Lumina
{
	namespace Input
	{
		enum class KeyState : int
		{
			None = -1,
			Released = 0,
			Pressed = 1,
			Held = 2
		};

		enum class KeyCode : int
		{
			Unknown = 0,

			// Printable keys
			Space = 32,
			Apostrophe = 39,
			Comma = 44,
			Minus = 45,
			Period = 46,
			Slash = 47,

			Num0 = 48, Num1 = 49, Num2 = 50, Num3 = 51, Num4 = 52,
			Num5 = 53, Num6 = 54, Num7 = 55, Num8 = 56, Num9 = 57,

			Semicolon = 59,
			Equal = 61,

			A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71, H = 72,
			I = 73, J = 74, K = 75, L = 76, M = 77, N = 78, O = 79, P = 80,
			Q = 81, R = 82, S = 83, T = 84, U = 85, V = 86, W = 87, X = 88,
			Y = 89, Z = 90,

			LeftBracket = 91,
			Backslash = 92,
			RightBracket = 93,
			GraveAccent = 96,
			World1 = 161,
			World2 = 162,

			// Function keys
			Escape = 256,
			Enter = 257,
			Tab = 258,
			Backspace = 259,
			Insert = 260,
			Delete = 261,
			Right = 262,
			Left = 263,
			Down = 264,
			Up = 265,
			PageUp = 266,
			PageDown = 267,
			Home = 268,
			End = 269,

			CapsLock = 280,
			ScrollLock = 281,
			NumLock = 282,
			PrintScreen = 283,
			Pause = 284,

			F1 = 290, F2 = 291, F3 = 292, F4 = 293, F5 = 294, F6 = 295, F7 = 296,
			F8 = 297, F9 = 298, F10 = 299, F11 = 300, F12 = 301, F13 = 302, F14 = 303,
			F15 = 304, F16 = 305, F17 = 306, F18 = 307, F19 = 308, F20 = 309, F21 = 310,
			F22 = 311, F23 = 312, F24 = 313, F25 = 314,

			// Keypad
			KP0 = 320, KP1 = 321, KP2 = 322, KP3 = 323, KP4 = 324,
			KP5 = 325, KP6 = 326, KP7 = 327, KP8 = 328, KP9 = 329,
			KPDecimal = 330,
			KPDivide = 331,
			KPMultiply = 332,
			KPSubtract = 333,
			KPAdd = 334,
			KPEnter = 335,
			KPEqual = 336,

			// Modifier keys
			LeftShift = 340,
			LeftControl = 341,
			LeftAlt = 342,
			LeftSuper = 343,
			RightShift = 344,
			RightControl = 345,
			RightAlt = 346,
			RightSuper = 347,

			Menu = 348
		};

		enum class CursorMode : int
		{
			Normal = 0,
			Hidden = 1,
			Disabled = 2
		};

		enum class MouseCode : int
		{
			Unknown = -1,
			Button0 = 0,
			Button1 = 1,
			Button2 = 2,
			Button3 = 3,
			Button4 = 4,
			Button5 = 5,
			Button6 = 6,
			Button7 = 7,

			Left = Button0,
			Right = Button1,
			Middle = Button2,
			Last = Button7
		};

		// Key state queries
		[[nodiscard]] bool IsKeyPressed(KeyCode keycode);
		[[nodiscard]] bool IsMouseButtonPressed(MouseCode button);

		// Cursor operations
		void SetCursorMode(CursorMode mode);

		struct MousePosition { float X, Y; };
		[[nodiscard]] MousePosition GetMousePosition();
		[[nodiscard]] inline float GetMouseX() { return GetMousePosition().X; }
		[[nodiscard]] inline float GetMouseY() { return GetMousePosition().Y; }

		// Key code utilities
		[[nodiscard]] constexpr bool IsModifierKey(KeyCode keycode)
		{
			return keycode == KeyCode::LeftShift || keycode == KeyCode::RightShift ||
			       keycode == KeyCode::LeftControl || keycode == KeyCode::RightControl ||
			       keycode == KeyCode::LeftAlt || keycode == KeyCode::RightAlt ||
			       keycode == KeyCode::LeftSuper || keycode == KeyCode::RightSuper;
		}

		[[nodiscard]] constexpr bool IsPrintableKey(KeyCode keycode)
		{
			return (keycode >= KeyCode::Space && keycode <= KeyCode::GraveAccent) ||
			       (keycode >= KeyCode::World1 && keycode <= KeyCode::World2);
		}

		[[nodiscard]] constexpr bool IsFunctionKey(KeyCode keycode)
		{
			return keycode >= KeyCode::F1 && keycode <= KeyCode::F25;
		}

		[[nodiscard]] constexpr bool IsArrowKey(KeyCode keycode)
		{
			return keycode == KeyCode::Up || keycode == KeyCode::Down ||
			       keycode == KeyCode::Left || keycode == KeyCode::Right;
		}

		[[nodiscard]] constexpr bool IsNumpadKey(KeyCode keycode)
		{
			return keycode >= KeyCode::KP0 && keycode <= KeyCode::KPEqual;
		}

		[[nodiscard]] constexpr bool IsLetterKey(KeyCode keycode)
		{
			return keycode >= KeyCode::A && keycode <= KeyCode::Z;
		}

		[[nodiscard]] constexpr bool IsNumberKey(KeyCode keycode)
		{
			return keycode >= KeyCode::Num0 && keycode <= KeyCode::Num9;
		}

		[[nodiscard]] constexpr std::string_view KeyStateToString(KeyState state)
		{
			switch (state)
			{
			case KeyState::None: return "None";
			case KeyState::Pressed: return "Pressed";
			case KeyState::Held: return "Held";
			case KeyState::Released: return "Released";
			default: return "Unknown";
			}
		}

		[[nodiscard]] constexpr std::string_view KeyCodeToString(KeyCode keycode)
		{
			switch (keycode)
			{
			case KeyCode::Unknown: return "Unknown";
			case KeyCode::Space: return "Space";
			case KeyCode::Apostrophe: return "Apostrophe";
			case KeyCode::Comma: return "Comma";
			case KeyCode::Minus: return "Minus";
			case KeyCode::Period: return "Period";
			case KeyCode::Slash: return "Slash";
			case KeyCode::Num0: return "0";
			case KeyCode::Num1: return "1";
			case KeyCode::Num2: return "2";
			case KeyCode::Num3: return "3";
			case KeyCode::Num4: return "4";
			case KeyCode::Num5: return "5";
			case KeyCode::Num6: return "6";
			case KeyCode::Num7: return "7";
			case KeyCode::Num8: return "8";
			case KeyCode::Num9: return "9";
			case KeyCode::Semicolon: return "Semicolon";
			case KeyCode::Equal: return "Equal";
			case KeyCode::A: return "A";
			case KeyCode::B: return "B";
			case KeyCode::C: return "C";
			case KeyCode::D: return "D";
			case KeyCode::E: return "E";
			case KeyCode::F: return "F";
			case KeyCode::G: return "G";
			case KeyCode::H: return "H";
			case KeyCode::I: return "I";
			case KeyCode::J: return "J";
			case KeyCode::K: return "K";
			case KeyCode::L: return "L";
			case KeyCode::M: return "M";
			case KeyCode::N: return "N";
			case KeyCode::O: return "O";
			case KeyCode::P: return "P";
			case KeyCode::Q: return "Q";
			case KeyCode::R: return "R";
			case KeyCode::S: return "S";
			case KeyCode::T: return "T";
			case KeyCode::U: return "U";
			case KeyCode::V: return "V";
			case KeyCode::W: return "W";
			case KeyCode::X: return "X";
			case KeyCode::Y: return "Y";
			case KeyCode::Z: return "Z";
			case KeyCode::LeftBracket: return "LeftBracket";
			case KeyCode::Backslash: return "Backslash";
			case KeyCode::RightBracket: return "RightBracket";
			case KeyCode::GraveAccent: return "GraveAccent";
			case KeyCode::World1: return "World1";
			case KeyCode::World2: return "World2";
			case KeyCode::Escape: return "Escape";
			case KeyCode::Enter: return "Enter";
			case KeyCode::Tab: return "Tab";
			case KeyCode::Backspace: return "Backspace";
			case KeyCode::Insert: return "Insert";
			case KeyCode::Delete: return "Delete";
			case KeyCode::Right: return "Right";
			case KeyCode::Left: return "Left";
			case KeyCode::Down: return "Down";
			case KeyCode::Up: return "Up";
			case KeyCode::PageUp: return "PageUp";
			case KeyCode::PageDown: return "PageDown";
			case KeyCode::Home: return "Home";
			case KeyCode::End: return "End";
			case KeyCode::CapsLock: return "CapsLock";
			case KeyCode::ScrollLock: return "ScrollLock";
			case KeyCode::NumLock: return "NumLock";
			case KeyCode::PrintScreen: return "PrintScreen";
			case KeyCode::Pause: return "Pause";
			case KeyCode::F1: return "F1";
			case KeyCode::F2: return "F2";
			case KeyCode::F3: return "F3";
			case KeyCode::F4: return "F4";
			case KeyCode::F5: return "F5";
			case KeyCode::F6: return "F6";
			case KeyCode::F7: return "F7";
			case KeyCode::F8: return "F8";
			case KeyCode::F9: return "F9";
			case KeyCode::F10: return "F10";
			case KeyCode::F11: return "F11";
			case KeyCode::F12: return "F12";
			case KeyCode::F13: return "F13";
			case KeyCode::F14: return "F14";
			case KeyCode::F15: return "F15";
			case KeyCode::F16: return "F16";
			case KeyCode::F17: return "F17";
			case KeyCode::F18: return "F18";
			case KeyCode::F19: return "F19";
			case KeyCode::F20: return "F20";
			case KeyCode::F21: return "F21";
			case KeyCode::F22: return "F22";
			case KeyCode::F23: return "F23";
			case KeyCode::F24: return "F24";
			case KeyCode::F25: return "F25";
			case KeyCode::KP0: return "KP0";
			case KeyCode::KP1: return "KP1";
			case KeyCode::KP2: return "KP2";
			case KeyCode::KP3: return "KP3";
			case KeyCode::KP4: return "KP4";
			case KeyCode::KP5: return "KP5";
			case KeyCode::KP6: return "KP6";
			case KeyCode::KP7: return "KP7";
			case KeyCode::KP8: return "KP8";
			case KeyCode::KP9: return "KP9";
			case KeyCode::KPDecimal: return "KPDecimal";
			case KeyCode::KPDivide: return "KPDivide";
			case KeyCode::KPMultiply: return "KPMultiply";
			case KeyCode::KPSubtract: return "KPSubtract";
			case KeyCode::KPAdd: return "KPAdd";
			case KeyCode::KPEnter: return "KPEnter";
			case KeyCode::KPEqual: return "KPEqual";
			case KeyCode::LeftShift: return "LeftShift";
			case KeyCode::LeftControl: return "LeftControl";
			case KeyCode::LeftAlt: return "LeftAlt";
			case KeyCode::LeftSuper: return "LeftSuper";
			case KeyCode::RightShift: return "RightShift";
			case KeyCode::RightControl: return "RightControl";
			case KeyCode::RightAlt: return "RightAlt";
			case KeyCode::RightSuper: return "RightSuper";
			case KeyCode::Menu: return "Menu";
			default: return "Unknown";
			}
		}

		[[nodiscard]] constexpr std::string_view MouseCodeToString(MouseCode button)
		{
			switch (button)
			{
			case MouseCode::Unknown: return "Unknown";
			case MouseCode::Button0: return "Left";
			case MouseCode::Button1: return "Right";
			case MouseCode::Button2: return "Middle";
			case MouseCode::Button3: return "Button3";
			case MouseCode::Button4: return "Button4";
			case MouseCode::Button5: return "Button5";
			case MouseCode::Button6: return "Button6";
			case MouseCode::Button7: return "Button7";
			default: return "Unknown";
			}
		}

		[[nodiscard]] constexpr std::string_view CursorModeToString(CursorMode mode)
		{
			switch (mode)
			{
			case CursorMode::Normal: return "Normal";
			case CursorMode::Hidden: return "Hidden";
			case CursorMode::Disabled: return "Disabled";
			default: return "Unknown";
			}
		}
	}
}
