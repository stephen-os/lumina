#include "Lumina/Core/Input.h"
#include "Lumina/Core/Log.h"

#ifdef __APPLE__
#include <CoreGraphics/CoreGraphics.h>
#include <Carbon/Carbon.h>

namespace Lumina::Core::Input
{
    static CGKeyCode LuminaToMacOSKeyCode(KeyCode key)
    {
        switch (key)
        {
        case KeyCode::A: return kVK_ANSI_A;
        case KeyCode::B: return kVK_ANSI_B;
        case KeyCode::C: return kVK_ANSI_C;
        case KeyCode::D: return kVK_ANSI_D;
        case KeyCode::E: return kVK_ANSI_E;
        case KeyCode::F: return kVK_ANSI_F;
        case KeyCode::G: return kVK_ANSI_G;
        case KeyCode::H: return kVK_ANSI_H;
        case KeyCode::I: return kVK_ANSI_I;
        case KeyCode::J: return kVK_ANSI_J;
        case KeyCode::K: return kVK_ANSI_K;
        case KeyCode::L: return kVK_ANSI_L;
        case KeyCode::M: return kVK_ANSI_M;
        case KeyCode::N: return kVK_ANSI_N;
        case KeyCode::O: return kVK_ANSI_O;
        case KeyCode::P: return kVK_ANSI_P;
        case KeyCode::Q: return kVK_ANSI_Q;
        case KeyCode::R: return kVK_ANSI_R;
        case KeyCode::S: return kVK_ANSI_S;
        case KeyCode::T: return kVK_ANSI_T;
        case KeyCode::U: return kVK_ANSI_U;
        case KeyCode::V: return kVK_ANSI_V;
        case KeyCode::W: return kVK_ANSI_W;
        case KeyCode::X: return kVK_ANSI_X;
        case KeyCode::Y: return kVK_ANSI_Y;
        case KeyCode::Z: return kVK_ANSI_Z;
        case KeyCode::Num0: return kVK_ANSI_0;
        case KeyCode::Num1: return kVK_ANSI_1;
        case KeyCode::Num2: return kVK_ANSI_2;
        case KeyCode::Num3: return kVK_ANSI_3;
        case KeyCode::Num4: return kVK_ANSI_4;
        case KeyCode::Num5: return kVK_ANSI_5;
        case KeyCode::Num6: return kVK_ANSI_6;
        case KeyCode::Num7: return kVK_ANSI_7;
        case KeyCode::Num8: return kVK_ANSI_8;
        case KeyCode::Num9: return kVK_ANSI_9;
        case KeyCode::F1: return kVK_F1;
        case KeyCode::F2: return kVK_F2;
        case KeyCode::F3: return kVK_F3;
        case KeyCode::F4: return kVK_F4;
        case KeyCode::F5: return kVK_F5;
        case KeyCode::F6: return kVK_F6;
        case KeyCode::F7: return kVK_F7;
        case KeyCode::F8: return kVK_F8;
        case KeyCode::F9: return kVK_F9;
        case KeyCode::F10: return kVK_F10;
        case KeyCode::F11: return kVK_F11;
        case KeyCode::F12: return kVK_F12;
        case KeyCode::Space: return kVK_Space;
        case KeyCode::Apostrophe: return kVK_ANSI_Quote;
        case KeyCode::Comma: return kVK_ANSI_Comma;
        case KeyCode::Minus: return kVK_ANSI_Minus;
        case KeyCode::Period: return kVK_ANSI_Period;
        case KeyCode::Slash: return kVK_ANSI_Slash;
        case KeyCode::Semicolon: return kVK_ANSI_Semicolon;
        case KeyCode::Equal: return kVK_ANSI_Equal;
        case KeyCode::LeftBracket: return kVK_ANSI_LeftBracket;
        case KeyCode::Backslash: return kVK_ANSI_Backslash;
        case KeyCode::RightBracket: return kVK_ANSI_RightBracket;
        case KeyCode::GraveAccent: return kVK_ANSI_Grave;
        case KeyCode::Escape: return kVK_Escape;
        case KeyCode::Enter: return kVK_Return;
        case KeyCode::Tab: return kVK_Tab;
        case KeyCode::Backspace: return kVK_Delete;
        case KeyCode::Insert: return kVK_Help;
        case KeyCode::Delete: return kVK_ForwardDelete;
        case KeyCode::Right: return kVK_RightArrow;
        case KeyCode::Left: return kVK_LeftArrow;
        case KeyCode::Down: return kVK_DownArrow;
        case KeyCode::Up: return kVK_UpArrow;
        case KeyCode::PageUp: return kVK_PageUp;
        case KeyCode::PageDown: return kVK_PageDown;
        case KeyCode::Home: return kVK_Home;
        case KeyCode::End: return kVK_End;
        case KeyCode::CapsLock: return kVK_CapsLock;
        case KeyCode::KP0: return kVK_ANSI_Keypad0;
        case KeyCode::KP1: return kVK_ANSI_Keypad1;
        case KeyCode::KP2: return kVK_ANSI_Keypad2;
        case KeyCode::KP3: return kVK_ANSI_Keypad3;
        case KeyCode::KP4: return kVK_ANSI_Keypad4;
        case KeyCode::KP5: return kVK_ANSI_Keypad5;
        case KeyCode::KP6: return kVK_ANSI_Keypad6;
        case KeyCode::KP7: return kVK_ANSI_Keypad7;
        case KeyCode::KP8: return kVK_ANSI_Keypad8;
        case KeyCode::KP9: return kVK_ANSI_Keypad9;
        case KeyCode::KPDecimal: return kVK_ANSI_KeypadDecimal;
        case KeyCode::KPDivide: return kVK_ANSI_KeypadDivide;
        case KeyCode::KPMultiply: return kVK_ANSI_KeypadMultiply;
        case KeyCode::KPSubtract: return kVK_ANSI_KeypadMinus;
        case KeyCode::KPAdd: return kVK_ANSI_KeypadPlus;
        case KeyCode::KPEnter: return kVK_ANSI_KeypadEnter;
        case KeyCode::KPEqual: return kVK_ANSI_KeypadEquals;
        case KeyCode::LeftShift: return kVK_Shift;
        case KeyCode::LeftControl: return kVK_Control;
        case KeyCode::LeftAlt: return kVK_Option;
        case KeyCode::LeftSuper: return kVK_Command;
        case KeyCode::RightShift: return kVK_RightShift;
        case KeyCode::RightControl: return kVK_RightControl;
        case KeyCode::RightAlt: return kVK_RightOption;
        case KeyCode::RightSuper: return kVK_RightCommand;
        default:
            LUMINA_LOG_WARN("Unmapped key: {}", static_cast<int>(key));
            return 0xFF;
        }
    }

    void PlatformPressKey(KeyCode key, bool press)
    {
        CGKeyCode keycode = LuminaToMacOSKeyCode(key);
        if (keycode == 0xFF)
            return;

        CGEventRef event = CGEventCreateKeyboardEvent(nullptr, keycode, press);
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
    }

    void PlatformPressMouseButton(MouseCode button, bool press)
    {
        CGEventType eventType;
        CGMouseButton cgButton;

        switch (button)
        {
        case MouseCode::Left:
            eventType = press ? kCGEventLeftMouseDown : kCGEventLeftMouseUp;
            cgButton = kCGMouseButtonLeft;
            break;
        case MouseCode::Right:
            eventType = press ? kCGEventRightMouseDown : kCGEventRightMouseUp;
            cgButton = kCGMouseButtonRight;
            break;
        case MouseCode::Middle:
            eventType = press ? kCGEventOtherMouseDown : kCGEventOtherMouseUp;
            cgButton = kCGMouseButtonCenter;
            break;
        default:
            LUMINA_LOG_WARN("Unsupported mouse button: {}", static_cast<int>(button));
            return;
        }

        CGPoint location = CGEventGetLocation(CGEventCreate(nullptr));
        CGEventRef event = CGEventCreateMouseEvent(nullptr, eventType, location, cgButton);
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
    }

    void PlatformMoveMouse(int x, int y, bool absolute)
    {
        CGPoint point;

        if (absolute)
        {
            point = CGPointMake(x, y);
        }
        else
        {
            point = CGEventGetLocation(CGEventCreate(nullptr));
            point.x += x;
            point.y += y;
        }

        CGEventRef event = CGEventCreateMouseEvent(nullptr, kCGEventMouseMoved, point, kCGMouseButtonLeft);
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
    }

    void PlatformScrollMouse(float xOffset, float yOffset)
    {
        CGEventRef event = CGEventCreateScrollWheelEvent(
            nullptr,
            kCGScrollEventUnitPixel,
            2,
            static_cast<int32_t>(yOffset),
            static_cast<int32_t>(xOffset)
        );

        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
    }
}

#endif