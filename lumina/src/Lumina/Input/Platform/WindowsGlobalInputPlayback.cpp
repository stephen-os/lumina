#include "WindowsGlobalInputPlayback.h"

#ifdef _WIN32

#include "Lumina/Core/Log.h"

namespace Lumina
{
    void WindowsGlobalInputPlayback::SimulateKeyPress(KeyCode key)
    {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = KeyCodeToVirtualKey(key);
        input.ki.dwFlags = 0; // 0 = key down

        SendInput(1, &input, sizeof(INPUT));
    }

    void WindowsGlobalInputPlayback::SimulateKeyRelease(KeyCode key)
    {
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = KeyCodeToVirtualKey(key);
        input.ki.dwFlags = KEYEVENTF_KEYUP;

        SendInput(1, &input, sizeof(INPUT));
    }

    void WindowsGlobalInputPlayback::SimulateMouseButtonPress(MouseCode button, int x, int y)
    {
        SendMouseButton(button, true, x, y);
    }

    void WindowsGlobalInputPlayback::SimulateMouseButtonRelease(MouseCode button, int x, int y)
    {
        SendMouseButton(button, false, x, y);
    }

    void WindowsGlobalInputPlayback::SimulateMouseMove(int x, int y)
    {
        // Convert to absolute coordinates (0-65535 range)
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        int absoluteX = (x * 65535) / screenWidth;
        int absoluteY = (y * 65535) / screenHeight;

        INPUT input = { 0 };
        input.type = INPUT_MOUSE;
        input.mi.dx = absoluteX;
        input.mi.dy = absoluteY;
        input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

        SendInput(1, &input, sizeof(INPUT));
    }

    void WindowsGlobalInputPlayback::SimulateMouseScroll(int dx, int dy)
    {
        if (dy != 0)
        {
            INPUT input = { 0 };
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_WHEEL;
            input.mi.mouseData = dy * WHEEL_DELTA;

            SendInput(1, &input, sizeof(INPUT));
        }

        if (dx != 0)
        {
            INPUT input = { 0 };
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_HWHEEL;
            input.mi.mouseData = dx * WHEEL_DELTA;

            SendInput(1, &input, sizeof(INPUT));
        }
    }

    WORD WindowsGlobalInputPlayback::KeyCodeToVirtualKey(KeyCode key)
    {
        switch (key)
        {
            // Function keys
        case KeyCode::F1: return VK_F1;
        case KeyCode::F2: return VK_F2;
        case KeyCode::F3: return VK_F3;
        case KeyCode::F4: return VK_F4;
        case KeyCode::F5: return VK_F5;
        case KeyCode::F6: return VK_F6;
        case KeyCode::F7: return VK_F7;
        case KeyCode::F8: return VK_F8;
        case KeyCode::F9: return VK_F9;
        case KeyCode::F10: return VK_F10;
        case KeyCode::F11: return VK_F11;
        case KeyCode::F12: return VK_F12;
        case KeyCode::F13: return VK_F13;
        case KeyCode::F14: return VK_F14;
        case KeyCode::F15: return VK_F15;
        case KeyCode::F16: return VK_F16;
        case KeyCode::F17: return VK_F17;
        case KeyCode::F18: return VK_F18;
        case KeyCode::F19: return VK_F19;
        case KeyCode::F20: return VK_F20;
        case KeyCode::F21: return VK_F21;
        case KeyCode::F22: return VK_F22;
        case KeyCode::F23: return VK_F23;
        case KeyCode::F24: return VK_F24;

            // Special keys
        case KeyCode::Escape: return VK_ESCAPE;
        case KeyCode::Enter: return VK_RETURN;
        case KeyCode::Tab: return VK_TAB;
        case KeyCode::Backspace: return VK_BACK;
        case KeyCode::Insert: return VK_INSERT;
        case KeyCode::Delete: return VK_DELETE;
        case KeyCode::Right: return VK_RIGHT;
        case KeyCode::Left: return VK_LEFT;
        case KeyCode::Down: return VK_DOWN;
        case KeyCode::Up: return VK_UP;
        case KeyCode::PageUp: return VK_PRIOR;
        case KeyCode::PageDown: return VK_NEXT;
        case KeyCode::Home: return VK_HOME;
        case KeyCode::End: return VK_END;
        case KeyCode::CapsLock: return VK_CAPITAL;
        case KeyCode::ScrollLock: return VK_SCROLL;
        case KeyCode::NumLock: return VK_NUMLOCK;
        case KeyCode::PrintScreen: return VK_SNAPSHOT;
        case KeyCode::Pause: return VK_PAUSE;

            // Modifiers
        case KeyCode::LeftShift: return VK_LSHIFT;
        case KeyCode::RightShift: return VK_RSHIFT;
        case KeyCode::LeftControl: return VK_LCONTROL;
        case KeyCode::RightControl: return VK_RCONTROL;
        case KeyCode::LeftAlt: return VK_LMENU;
        case KeyCode::RightAlt: return VK_RMENU;
        case KeyCode::LeftSuper: return VK_LWIN;
        case KeyCode::RightSuper: return VK_RWIN;
        case KeyCode::Menu: return VK_APPS;

            // Numpad
        case KeyCode::KP0: return VK_NUMPAD0;
        case KeyCode::KP1: return VK_NUMPAD1;
        case KeyCode::KP2: return VK_NUMPAD2;
        case KeyCode::KP3: return VK_NUMPAD3;
        case KeyCode::KP4: return VK_NUMPAD4;
        case KeyCode::KP5: return VK_NUMPAD5;
        case KeyCode::KP6: return VK_NUMPAD6;
        case KeyCode::KP7: return VK_NUMPAD7;
        case KeyCode::KP8: return VK_NUMPAD8;
        case KeyCode::KP9: return VK_NUMPAD9;
        case KeyCode::KPDecimal: return VK_DECIMAL;
        case KeyCode::KPDivide: return VK_DIVIDE;
        case KeyCode::KPMultiply: return VK_MULTIPLY;
        case KeyCode::KPSubtract: return VK_SUBTRACT;
        case KeyCode::KPAdd: return VK_ADD;

            // Printable characters
        case KeyCode::Space: return VK_SPACE;
        case KeyCode::Apostrophe: return VK_OEM_7;
        case KeyCode::Comma: return VK_OEM_COMMA;
        case KeyCode::Minus: return VK_OEM_MINUS;
        case KeyCode::Period: return VK_OEM_PERIOD;
        case KeyCode::Slash: return VK_OEM_2;
        case KeyCode::Semicolon: return VK_OEM_1;
        case KeyCode::Equal: return VK_OEM_PLUS;
        case KeyCode::LeftBracket: return VK_OEM_4;
        case KeyCode::Backslash: return VK_OEM_5;
        case KeyCode::RightBracket: return VK_OEM_6;
        case KeyCode::GraveAccent: return VK_OEM_3;

            // Numbers 0-9
        case KeyCode::Num0: return 0x30;
        case KeyCode::Num1: return 0x31;
        case KeyCode::Num2: return 0x32;
        case KeyCode::Num3: return 0x33;
        case KeyCode::Num4: return 0x34;
        case KeyCode::Num5: return 0x35;
        case KeyCode::Num6: return 0x36;
        case KeyCode::Num7: return 0x37;
        case KeyCode::Num8: return 0x38;
        case KeyCode::Num9: return 0x39;

            // Letters A-Z
        case KeyCode::A: return 0x41;
        case KeyCode::B: return 0x42;
        case KeyCode::C: return 0x43;
        case KeyCode::D: return 0x44;
        case KeyCode::E: return 0x45;
        case KeyCode::F: return 0x46;
        case KeyCode::G: return 0x47;
        case KeyCode::H: return 0x48;
        case KeyCode::I: return 0x49;
        case KeyCode::J: return 0x4A;
        case KeyCode::K: return 0x4B;
        case KeyCode::L: return 0x4C;
        case KeyCode::M: return 0x4D;
        case KeyCode::N: return 0x4E;
        case KeyCode::O: return 0x4F;
        case KeyCode::P: return 0x50;
        case KeyCode::Q: return 0x51;
        case KeyCode::R: return 0x52;
        case KeyCode::S: return 0x53;
        case KeyCode::T: return 0x54;
        case KeyCode::U: return 0x55;
        case KeyCode::V: return 0x56;
        case KeyCode::W: return 0x57;
        case KeyCode::X: return 0x58;
        case KeyCode::Y: return 0x59;
        case KeyCode::Z: return 0x5A;

        default:
            return 0;
        }
    }

    void WindowsGlobalInputPlayback::SendMouseButton(MouseCode button, bool pressed, int x, int y)
    {
        // Move mouse to position first
        SimulateMouseMove(x, y);

        INPUT input = { 0 };
        input.type = INPUT_MOUSE;

        switch (button)
        {
        case MouseCode::Left:
            input.mi.dwFlags = pressed ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
            break;
        case MouseCode::Right:
            input.mi.dwFlags = pressed ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
            break;
        case MouseCode::Middle:
            input.mi.dwFlags = pressed ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
            break;
        case MouseCode::Button3:
        case MouseCode::Button4:
            input.mi.dwFlags = pressed ? MOUSEEVENTF_XDOWN : MOUSEEVENTF_XUP;
            input.mi.mouseData = (button == MouseCode::Button3) ? XBUTTON1 : XBUTTON2;
            break;
        default:
            return;
        }

        SendInput(1, &input, sizeof(INPUT));
    }
}

#endif // _WIN32