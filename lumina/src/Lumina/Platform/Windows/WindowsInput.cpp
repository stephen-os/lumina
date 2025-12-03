#include "Lumina/Core/Input.h"
#include "Lumina/Core/Log.h"

#ifdef _WIN32
#include <Windows.h>

namespace Lumina::Core::Input
{
    static WORD LuminaToWindowsVK(KeyCode key)
    {
        switch (key)
        {
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
        case KeyCode::KPEnter: return VK_RETURN;
        case KeyCode::KPEqual: return VK_OEM_PLUS;
        case KeyCode::LeftShift: return VK_LSHIFT;
        case KeyCode::LeftControl: return VK_LCONTROL;
        case KeyCode::LeftAlt: return VK_LMENU;
        case KeyCode::LeftSuper: return VK_LWIN;
        case KeyCode::RightShift: return VK_RSHIFT;
        case KeyCode::RightControl: return VK_RCONTROL;
        case KeyCode::RightAlt: return VK_RMENU;
        case KeyCode::RightSuper: return VK_RWIN;
        case KeyCode::Menu: return VK_APPS;
        default:
            LUMINA_LOG_WARN("Unmapped key: {}", static_cast<int>(key));
            return 0;
        }
    }

    void PlatformPressKey(KeyCode key, bool press)
    {
        WORD vk = LuminaToWindowsVK(key);
        if (vk == 0)
            return;

        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;
        input.ki.wVk = vk;
        input.ki.dwFlags = press ? 0 : KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(INPUT));
    }

    void PlatformPressMouseButton(MouseCode button, bool press)
    {
        INPUT input = { 0 };
        input.type = INPUT_MOUSE;

        switch (button)
        {
        case MouseCode::Left:
            input.mi.dwFlags = press ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
            break;
        case MouseCode::Right:
            input.mi.dwFlags = press ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
            break;
        case MouseCode::Middle:
            input.mi.dwFlags = press ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            LUMINA_LOG_WARN("Unsupported mouse button: {}", static_cast<int>(button));
            return;
        }

        SendInput(1, &input, sizeof(INPUT));
    }

    void PlatformMoveMouse(int x, int y, bool absolute)
    {
        INPUT input = { 0 };
        input.type = INPUT_MOUSE;

        if (absolute)
        {
            input.mi.dx = (x * 65535) / GetSystemMetrics(SM_CXSCREEN);
            input.mi.dy = (y * 65535) / GetSystemMetrics(SM_CYSCREEN);
            input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
        }
        else
        {
            input.mi.dx = x;
            input.mi.dy = y;
            input.mi.dwFlags = MOUSEEVENTF_MOVE;
        }

        SendInput(1, &input, sizeof(INPUT));
    }

    void PlatformScrollMouse(float xOffset, float yOffset)
    {
        if (yOffset != 0.0f)
        {
            INPUT input = { 0 };
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_WHEEL;
            input.mi.mouseData = static_cast<DWORD>(yOffset * WHEEL_DELTA);
            SendInput(1, &input, sizeof(INPUT));
        }

        if (xOffset != 0.0f)
        {
            INPUT input = { 0 };
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_HWHEEL;
            input.mi.mouseData = static_cast<DWORD>(xOffset * WHEEL_DELTA);
            SendInput(1, &input, sizeof(INPUT));
        }
    }
}

#endif