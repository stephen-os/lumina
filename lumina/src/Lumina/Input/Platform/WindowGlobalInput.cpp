#include "WindowsGlobalInput.h"

#ifdef _WIN32

#include "Lumina/Core/Log.h"

namespace Lumina
{
    WindowsGlobalInput* WindowsGlobalInput::s_Instance = nullptr;

    WindowsGlobalInput::WindowsGlobalInput()
    {
        s_Instance = this;
    }

    WindowsGlobalInput::~WindowsGlobalInput()
    {
        Stop();
        s_Instance = nullptr;
    }

    bool WindowsGlobalInput::Start()
    {
        if (m_IsActive)
        {
            LUMINA_LOG_WARN("WindowsGlobalInput already active");
            return true;
        }

        // Install keyboard hook
        m_KeyboardHook = SetWindowsHookEx(
            WH_KEYBOARD_LL,
            KeyboardHookProc,
            GetModuleHandle(NULL),
            0
        );

        if (!m_KeyboardHook)
        {
            LUMINA_LOG_ERROR("Failed to install keyboard hook. Error: {}", GetLastError());
            return false;
        }

        // Install mouse hook
        m_MouseHook = SetWindowsHookEx(
            WH_MOUSE_LL,
            MouseHookProc,
            GetModuleHandle(NULL),
            0
        );

        if (!m_MouseHook)
        {
            LUMINA_LOG_ERROR("Failed to install mouse hook. Error: {}", GetLastError());
            UnhookWindowsHookEx(m_KeyboardHook);
            m_KeyboardHook = nullptr;
            return false;
        }

        m_IsActive = true;
        LUMINA_LOG_INFO("WindowsGlobalInput started successfully");
        return true;
    }

    void WindowsGlobalInput::Stop()
    {
        if (!m_IsActive)
            return;

        if (m_KeyboardHook)
        {
            UnhookWindowsHookEx(m_KeyboardHook);
            m_KeyboardHook = nullptr;
        }

        if (m_MouseHook)
        {
            UnhookWindowsHookEx(m_MouseHook);
            m_MouseHook = nullptr;
        }

        m_IsActive = false;
        LUMINA_LOG_INFO("WindowsGlobalInput stopped");
    }

    LRESULT CALLBACK WindowsGlobalInput::KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        if (nCode >= 0 && s_Instance)
        {
            KBDLLHOOKSTRUCT* kbData = (KBDLLHOOKSTRUCT*)lParam;
            bool pressed = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

            KeyCode keyCode = VirtualKeyToKeyCode(kbData->vkCode);

            std::lock_guard<std::mutex> lock(s_Instance->m_CallbackMutex);

            if (s_Instance->m_KeyCallback)
            {
                s_Instance->m_KeyCallback(keyCode, pressed);
            }

            s_Instance->PostGlobalKeyEvent(keyCode, pressed);
        }

        return CallNextHookEx(s_Instance ? s_Instance->m_KeyboardHook : NULL, nCode, wParam, lParam);
    }

    LRESULT CALLBACK WindowsGlobalInput::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        if (nCode >= 0 && s_Instance)
        {
            MSLLHOOKSTRUCT* msData = (MSLLHOOKSTRUCT*)lParam;

            std::lock_guard<std::mutex> lock(s_Instance->m_CallbackMutex);

            switch (wParam)
            {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
            {
                MouseCode button = ButtonToMouseCode(wParam);
                bool pressed = (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN ||
                    wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN);

                if (s_Instance->m_MouseButtonCallback)
                {
                    s_Instance->m_MouseButtonCallback(button, pressed, msData->pt.x, msData->pt.y);
                }

                s_Instance->PostGlobalMouseButtonEvent(button, pressed, msData->pt.x, msData->pt.y);
                break;
            }

            case WM_MOUSEMOVE:
            {
                if (s_Instance->m_MouseMoveCallback)
                {
                    s_Instance->m_MouseMoveCallback(msData->pt.x, msData->pt.y);
                }

                s_Instance->PostGlobalMouseMoveEvent(msData->pt.x, msData->pt.y);
                break;
            }

            case WM_MOUSEWHEEL:
            {
                short delta = GET_WHEEL_DELTA_WPARAM(msData->mouseData);
                int scrollAmount = delta / WHEEL_DELTA;

                if (s_Instance->m_ScrollCallback)
                {
                    s_Instance->m_ScrollCallback(0, scrollAmount);
                }

                s_Instance->PostGlobalMouseScrollEvent(0, scrollAmount);
                break;
            }

            case WM_MOUSEHWHEEL:
            {
                short delta = GET_WHEEL_DELTA_WPARAM(msData->mouseData);
                int scrollAmount = delta / WHEEL_DELTA;

                if (s_Instance->m_ScrollCallback)
                {
                    s_Instance->m_ScrollCallback(scrollAmount, 0);
                }

                s_Instance->PostGlobalMouseScrollEvent(scrollAmount, 0);
                break;
            }
            }
        }

        return CallNextHookEx(s_Instance ? s_Instance->m_MouseHook : NULL, nCode, wParam, lParam);
    }

    KeyCode WindowsGlobalInput::VirtualKeyToKeyCode(DWORD vkCode)
    {
        switch (vkCode)
        {
            // Function keys
        case VK_F1: return KeyCode::F1;
        case VK_F2: return KeyCode::F2;
        case VK_F3: return KeyCode::F3;
        case VK_F4: return KeyCode::F4;
        case VK_F5: return KeyCode::F5;
        case VK_F6: return KeyCode::F6;
        case VK_F7: return KeyCode::F7;
        case VK_F8: return KeyCode::F8;
        case VK_F9: return KeyCode::F9;
        case VK_F10: return KeyCode::F10;
        case VK_F11: return KeyCode::F11;
        case VK_F12: return KeyCode::F12;
        case VK_F13: return KeyCode::F13;
        case VK_F14: return KeyCode::F14;
        case VK_F15: return KeyCode::F15;
        case VK_F16: return KeyCode::F16;
        case VK_F17: return KeyCode::F17;
        case VK_F18: return KeyCode::F18;
        case VK_F19: return KeyCode::F19;
        case VK_F20: return KeyCode::F20;
        case VK_F21: return KeyCode::F21;
        case VK_F22: return KeyCode::F22;
        case VK_F23: return KeyCode::F23;
        case VK_F24: return KeyCode::F24;

            // Special keys
        case VK_ESCAPE: return KeyCode::Escape;
        case VK_RETURN: return KeyCode::Enter;
        case VK_TAB: return KeyCode::Tab;
        case VK_BACK: return KeyCode::Backspace;
        case VK_INSERT: return KeyCode::Insert;
        case VK_DELETE: return KeyCode::Delete;
        case VK_RIGHT: return KeyCode::Right;
        case VK_LEFT: return KeyCode::Left;
        case VK_DOWN: return KeyCode::Down;
        case VK_UP: return KeyCode::Up;
        case VK_PRIOR: return KeyCode::PageUp;
        case VK_NEXT: return KeyCode::PageDown;
        case VK_HOME: return KeyCode::Home;
        case VK_END: return KeyCode::End;
        case VK_CAPITAL: return KeyCode::CapsLock;
        case VK_SCROLL: return KeyCode::ScrollLock;
        case VK_NUMLOCK: return KeyCode::NumLock;
        case VK_SNAPSHOT: return KeyCode::PrintScreen;
        case VK_PAUSE: return KeyCode::Pause;

            // Modifiers
        case VK_LSHIFT: return KeyCode::LeftShift;
        case VK_RSHIFT: return KeyCode::RightShift;
        case VK_LCONTROL: return KeyCode::LeftControl;
        case VK_RCONTROL: return KeyCode::RightControl;
        case VK_LMENU: return KeyCode::LeftAlt;
        case VK_RMENU: return KeyCode::RightAlt;
        case VK_LWIN: return KeyCode::LeftSuper;
        case VK_RWIN: return KeyCode::RightSuper;
        case VK_APPS: return KeyCode::Menu;

            // Numpad
        case VK_NUMPAD0: return KeyCode::KP0;
        case VK_NUMPAD1: return KeyCode::KP1;
        case VK_NUMPAD2: return KeyCode::KP2;
        case VK_NUMPAD3: return KeyCode::KP3;
        case VK_NUMPAD4: return KeyCode::KP4;
        case VK_NUMPAD5: return KeyCode::KP5;
        case VK_NUMPAD6: return KeyCode::KP6;
        case VK_NUMPAD7: return KeyCode::KP7;
        case VK_NUMPAD8: return KeyCode::KP8;
        case VK_NUMPAD9: return KeyCode::KP9;
        case VK_DECIMAL: return KeyCode::KPDecimal;
        case VK_DIVIDE: return KeyCode::KPDivide;
        case VK_MULTIPLY: return KeyCode::KPMultiply;
        case VK_SUBTRACT: return KeyCode::KPSubtract;
        case VK_ADD: return KeyCode::KPAdd;

            // Printable characters
        case VK_SPACE: return KeyCode::Space;
        case VK_OEM_7: return KeyCode::Apostrophe;
        case VK_OEM_COMMA: return KeyCode::Comma;
        case VK_OEM_MINUS: return KeyCode::Minus;
        case VK_OEM_PERIOD: return KeyCode::Period;
        case VK_OEM_2: return KeyCode::Slash;
        case VK_OEM_1: return KeyCode::Semicolon;
        case VK_OEM_PLUS: return KeyCode::Equal;
        case VK_OEM_4: return KeyCode::LeftBracket;
        case VK_OEM_5: return KeyCode::Backslash;
        case VK_OEM_6: return KeyCode::RightBracket;
        case VK_OEM_3: return KeyCode::GraveAccent;

            // Numbers 0-9
        case 0x30: return KeyCode::Num0;
        case 0x31: return KeyCode::Num1;
        case 0x32: return KeyCode::Num2;
        case 0x33: return KeyCode::Num3;
        case 0x34: return KeyCode::Num4;
        case 0x35: return KeyCode::Num5;
        case 0x36: return KeyCode::Num6;
        case 0x37: return KeyCode::Num7;
        case 0x38: return KeyCode::Num8;
        case 0x39: return KeyCode::Num9;

            // Letters A-Z
        case 0x41: return KeyCode::A;
        case 0x42: return KeyCode::B;
        case 0x43: return KeyCode::C;
        case 0x44: return KeyCode::D;
        case 0x45: return KeyCode::E;
        case 0x46: return KeyCode::F;
        case 0x47: return KeyCode::G;
        case 0x48: return KeyCode::H;
        case 0x49: return KeyCode::I;
        case 0x4A: return KeyCode::J;
        case 0x4B: return KeyCode::K;
        case 0x4C: return KeyCode::L;
        case 0x4D: return KeyCode::M;
        case 0x4E: return KeyCode::N;
        case 0x4F: return KeyCode::O;
        case 0x50: return KeyCode::P;
        case 0x51: return KeyCode::Q;
        case 0x52: return KeyCode::R;
        case 0x53: return KeyCode::S;
        case 0x54: return KeyCode::T;
        case 0x55: return KeyCode::U;
        case 0x56: return KeyCode::V;
        case 0x57: return KeyCode::W;
        case 0x58: return KeyCode::X;
        case 0x59: return KeyCode::Y;
        case 0x5A: return KeyCode::Z;

        default:
            return KeyCode::Unknown;
        }
    }

    MouseCode WindowsGlobalInput::ButtonToMouseCode(WPARAM wParam)
    {
        switch (wParam)
        {
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            return MouseCode::Left;

        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
            return MouseCode::Right;

        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
            return MouseCode::Middle;

        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
            return MouseCode::Button3;

        default:
            return MouseCode::Button0;
        }
    }
}

#endif // _WIN32