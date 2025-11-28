#pragma once

#include <string_view>
#include <vector>

#include <glm/glm.hpp>

namespace Lumina::Core::Input
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
        Apostrophe = 39,  /* ' */
        Comma = 44,       /* , */
        Minus = 45,       /* - */
        Period = 46,      /* . */
        Slash = 47,       /* / */

        Num0 = 48, Num1 = 49, Num2 = 50, Num3 = 51, Num4 = 52,
        Num5 = 53, Num6 = 54, Num7 = 55, Num8 = 56, Num9 = 57,

        Semicolon = 59,   /* ; */
        Equal = 61,       /* = */

        A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71, H = 72,
        I = 73, J = 74, K = 75, L = 76, M = 77, N = 78, O = 79, P = 80,
        Q = 81, R = 82, S = 83, T = 84, U = 85, V = 86, W = 87, X = 88,
        Y = 89, Z = 90,

        LeftBracket = 91,  /* [ */
        Backslash = 92,    /* \ */
        RightBracket = 93, /* ] */
        GraveAccent = 96,  /* ` */
        World1 = 161,      /* non-US #1 */
        World2 = 162,      /* non-US #2 */

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

    class KeyCombo
    {
    public:
        KeyCombo() = default;
		KeyCombo(std::initializer_list<KeyCode> keys) : m_Keys(keys) {}
        KeyCombo(const std::vector<KeyCode>& keys) : m_Keys(keys) {}

        bool IsKeyComboPressed() const; 

        void SetKeyCombo(std::initializer_list<KeyCode> keys) { m_Keys = keys; }
		void SetKeyCombo(const std::vector<KeyCode>& keys) { m_Keys = keys; };

		bool AddKeyCode(KeyCode key);
		bool RemoveKeyCode(KeyCode key);

        void Clear() { m_Keys.clear(); };

        bool IsEmpty() const { return m_Keys.empty(); };
		bool IsKeyCodeInCombo(KeyCode key) const;

        size_t Size() const { return m_Keys.size(); };

    private:
		std::vector<KeyCode> m_Keys;
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

	// Key operations
	bool IsKeyPressed(KeyCode keycode);
    
    constexpr bool IsModifierKey(KeyCode keycode)
    {
        return keycode == KeyCode::LeftShift || keycode == KeyCode::RightShift ||
            keycode == KeyCode::LeftControl || keycode == KeyCode::RightControl ||
            keycode == KeyCode::LeftAlt || keycode == KeyCode::RightAlt ||
            keycode == KeyCode::LeftSuper || keycode == KeyCode::RightSuper;
    }

    constexpr bool IsPrintableKey(KeyCode keycode)
    {
        return (keycode >= KeyCode::Space && keycode <= KeyCode::GraveAccent) ||
            (keycode >= KeyCode::World1 && keycode <= KeyCode::World2);
    }

    constexpr bool IsFunctionKey(KeyCode keycode)
    {
        return keycode >= KeyCode::F1 && keycode <= KeyCode::F25;
    }

    constexpr bool IsArrowKey(KeyCode keycode)
    {
        return keycode == KeyCode::Up || keycode == KeyCode::Down ||
            keycode == KeyCode::Left || keycode == KeyCode::Right;
    }

    constexpr bool IsNumpadKey(KeyCode keycode)
    {
        return keycode >= KeyCode::KP0 && keycode <= KeyCode::KPEqual;
    }

    constexpr bool IsLetterKey(KeyCode keycode)
    {
        return keycode >= KeyCode::A && keycode <= KeyCode::Z;
    }

    constexpr bool IsNumberKey(KeyCode keycode)
    {
        return keycode >= KeyCode::Num0 && keycode <= KeyCode::Num9;
    }

    constexpr bool IsShiftKey(KeyCode keycode)
    {
        return keycode == KeyCode::LeftShift || keycode == KeyCode::RightShift;
    }

    constexpr bool IsControlKey(KeyCode keycode)
    {
        return keycode == KeyCode::LeftControl || keycode == KeyCode::RightControl;
    }

    constexpr bool IsAltKey(KeyCode keycode)
    {
        return keycode == KeyCode::LeftAlt || keycode == KeyCode::RightAlt;
    }

    constexpr bool IsSuperKey(KeyCode keycode)
    {
        return keycode == KeyCode::LeftSuper || keycode == KeyCode::RightSuper;
    }

    constexpr std::string_view KeyStateToString(KeyState state)
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

    constexpr std::string_view KeyCodeToString(KeyCode keycode)
    {
        switch (keycode)
        {
        case KeyCode::Unknown: return "Unknown";

            // Printable keys
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

        case KeyCode::LeftBracket: return "Left Bracket";
        case KeyCode::Backslash: return "Backslash";
        case KeyCode::RightBracket: return "Right Bracket";
        case KeyCode::GraveAccent: return "Grave Accent";
        case KeyCode::World1: return "World 1";
        case KeyCode::World2: return "World 2";

            // Function keys
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
        case KeyCode::PageUp: return "Page Up";
        case KeyCode::PageDown: return "Page Down";
        case KeyCode::Home: return "Home";
        case KeyCode::End: return "End";
        case KeyCode::CapsLock: return "Caps Lock";
        case KeyCode::ScrollLock: return "Scroll Lock";
        case KeyCode::NumLock: return "Num Lock";
        case KeyCode::PrintScreen: return "Print Screen";
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

            // Keypad
        case KeyCode::KP0: return "Keypad 0";
        case KeyCode::KP1: return "Keypad 1";
        case KeyCode::KP2: return "Keypad 2";
        case KeyCode::KP3: return "Keypad 3";
        case KeyCode::KP4: return "Keypad 4";
        case KeyCode::KP5: return "Keypad 5";
        case KeyCode::KP6: return "Keypad 6";
        case KeyCode::KP7: return "Keypad 7";
        case KeyCode::KP8: return "Keypad 8";
        case KeyCode::KP9: return "Keypad 9";
        case KeyCode::KPDecimal: return "Keypad Decimal";
        case KeyCode::KPDivide: return "Keypad Divide";
        case KeyCode::KPMultiply: return "Keypad Multiply";
        case KeyCode::KPSubtract: return "Keypad Subtract";
        case KeyCode::KPAdd: return "Keypad Add";
        case KeyCode::KPEnter: return "Keypad Enter";
        case KeyCode::KPEqual: return "Keypad Equal";

            // Modifier keys
        case KeyCode::LeftShift: return "Left Shift";
        case KeyCode::LeftControl: return "Left Control";
        case KeyCode::LeftAlt: return "Left Alt";
        case KeyCode::LeftSuper: return "Left Super";
        case KeyCode::RightShift: return "Right Shift";
        case KeyCode::RightControl: return "Right Control";
        case KeyCode::RightAlt: return "Right Alt";
        case KeyCode::RightSuper: return "Right Super";
        case KeyCode::Menu: return "Menu";

        default: return "Unknown";
        }
    }

	// Mouse operations
    bool IsMouseButtonPressed(MouseCode mousecode);
	void SetCursorMode(CursorMode mode);
	glm::vec2 GetMousePosition();
    inline float GetMouseX() { return GetMousePosition().x; };
    inline float GetMouseY() { return GetMousePosition().y; }; 

    constexpr std::string_view MouseCodeToString(MouseCode button)
    {
        switch (button)
        {
        case MouseCode::Unknown: return "Unknown";
        case MouseCode::Button0: return "Left";
        case MouseCode::Button1: return "Right";
        case MouseCode::Button2: return "Middle";
        case MouseCode::Button3: return "Button 3";
        case MouseCode::Button4: return "Button 4";
        case MouseCode::Button5: return "Button 5";
        case MouseCode::Button6: return "Button 6";
        case MouseCode::Button7: return "Button 7";
        default: return "Unknown";
        }
    }

    constexpr std::string_view CursorModeToString(CursorMode mode)
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