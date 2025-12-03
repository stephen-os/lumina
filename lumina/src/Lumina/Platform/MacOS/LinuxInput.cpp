#include "Lumina/Core/Input.h"
#include "Lumina/Core/Log.h"

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>

namespace Lumina::Core::Input
{
    static KeySym LuminaToX11KeySym(KeyCode key)
    {
        switch (key)
        {
        case KeyCode::A: return XK_a;
        case KeyCode::B: return XK_b;
        case KeyCode::C: return XK_c;
        case KeyCode::D: return XK_d;
        case KeyCode::E: return XK_e;
        case KeyCode::F: return XK_f;
        case KeyCode::G: return XK_g;
        case KeyCode::H: return XK_h;
        case KeyCode::I: return XK_i;
        case KeyCode::J: return XK_j;
        case KeyCode::K: return XK_k;
        case KeyCode::L: return XK_l;
        case KeyCode::M: return XK_m;
        case KeyCode::N: return XK_n;
        case KeyCode::O: return XK_o;
        case KeyCode::P: return XK_p;
        case KeyCode::Q: return XK_q;
        case KeyCode::R: return XK_r;
        case KeyCode::S: return XK_s;
        case KeyCode::T: return XK_t;
        case KeyCode::U: return XK_u;
        case KeyCode::V: return XK_v;
        case KeyCode::W: return XK_w;
        case KeyCode::X: return XK_x;
        case KeyCode::Y: return XK_y;
        case KeyCode::Z: return XK_z;
        case KeyCode::Num0: return XK_0;
        case KeyCode::Num1: return XK_1;
        case KeyCode::Num2: return XK_2;
        case KeyCode::Num3: return XK_3;
        case KeyCode::Num4: return XK_4;
        case KeyCode::Num5: return XK_5;
        case KeyCode::Num6: return XK_6;
        case KeyCode::Num7: return XK_7;
        case KeyCode::Num8: return XK_8;
        case KeyCode::Num9: return XK_9;
        case KeyCode::F1: return XK_F1;
        case KeyCode::F2: return XK_F2;
        case KeyCode::F3: return XK_F3;
        case KeyCode::F4: return XK_F4;
        case KeyCode::F5: return XK_F5;
        case KeyCode::F6: return XK_F6;
        case KeyCode::F7: return XK_F7;
        case KeyCode::F8: return XK_F8;
        case KeyCode::F9: return XK_F9;
        case KeyCode::F10: return XK_F10;
        case KeyCode::F11: return XK_F11;
        case KeyCode::F12: return XK_F12;
        case KeyCode::Space: return XK_space;
        case KeyCode::Apostrophe: return XK_apostrophe;
        case KeyCode::Comma: return XK_comma;
        case KeyCode::Minus: return XK_minus;
        case KeyCode::Period: return XK_period;
        case KeyCode::Slash: return XK_slash;
        case KeyCode::Semicolon: return XK_semicolon;
        case KeyCode::Equal: return XK_equal;
        case KeyCode::LeftBracket: return XK_bracketleft;
        case KeyCode::Backslash: return XK_backslash;
        case KeyCode::RightBracket: return XK_bracketright;
        case KeyCode::GraveAccent: return XK_grave;
        case KeyCode::Escape: return XK_Escape;
        case KeyCode::Enter: return XK_Return;
        case KeyCode::Tab: return XK_Tab;
        case KeyCode::Backspace: return XK_BackSpace;
        case KeyCode::Insert: return XK_Insert;
        case KeyCode::Delete: return XK_Delete;
        case KeyCode::Right: return XK_Right;
        case KeyCode::Left: return XK_Left;
        case KeyCode::Down: return XK_Down;
        case KeyCode::Up: return XK_Up;
        case KeyCode::PageUp: return XK_Page_Up;
        case KeyCode::PageDown: return XK_Page_Down;
        case KeyCode::Home: return XK_Home;
        case KeyCode::End: return XK_End;
        case KeyCode::CapsLock: return XK_Caps_Lock;
        case KeyCode::ScrollLock: return XK_Scroll_Lock;
        case KeyCode::NumLock: return XK_Num_Lock;
        case KeyCode::PrintScreen: return XK_Print;
        case KeyCode::Pause: return XK_Pause;
        case KeyCode::KP0: return XK_KP_0;
        case KeyCode::KP1: return XK_KP_1;
        case KeyCode::KP2: return XK_KP_2;
        case KeyCode::KP3: return XK_KP_3;
        case KeyCode::KP4: return XK_KP_4;
        case KeyCode::KP5: return XK_KP_5;
        case KeyCode::KP6: return XK_KP_6;
        case KeyCode::KP7: return XK_KP_7;
        case KeyCode::KP8: return XK_KP_8;
        case KeyCode::KP9: return XK_KP_9;
        case KeyCode::KPDecimal: return XK_KP_Decimal;
        case KeyCode::KPDivide: return XK_KP_Divide;
        case KeyCode::KPMultiply: return XK_KP_Multiply;
        case KeyCode::KPSubtract: return XK_KP_Subtract;
        case KeyCode::KPAdd: return XK_KP_Add;
        case KeyCode::KPEnter: return XK_KP_Enter;
        case KeyCode::KPEqual: return XK_KP_Equal;
        case KeyCode::LeftShift: return XK_Shift_L;
        case KeyCode::LeftControl: return XK_Control_L;
        case KeyCode::LeftAlt: return XK_Alt_L;
        case KeyCode::LeftSuper: return XK_Super_L;
        case KeyCode::RightShift: return XK_Shift_R;
        case KeyCode::RightControl: return XK_Control_R;
        case KeyCode::RightAlt: return XK_Alt_R;
        case KeyCode::RightSuper: return XK_Super_R;
        case KeyCode::Menu: return XK_Menu;
        default:
            LUMINA_LOG_WARN("Unmapped key: {}", static_cast<int>(key));
            return 0;
        }
    }

    void PlatformPressKey(KeyCode key, bool press)
    {
        Display* display = XOpenDisplay(nullptr);
        if (!display)
        {
            LUMINA_LOG_ERROR("Failed to open X11 display");
            return;
        }

        KeySym keysym = LuminaToX11KeySym(key);
        if (keysym == 0)
        {
            XCloseDisplay(display);
            return;
        }

        KeyCode keycode = XKeysymToKeycode(display, keysym);
        XTestFakeKeyEvent(display, keycode, press, 0);
        XFlush(display);
        XCloseDisplay(display);
    }

    void PlatformPressMouseButton(MouseCode button, bool press)
    {
        Display* display = XOpenDisplay(nullptr);
        if (!display)
        {
            LUMINA_LOG_ERROR("Failed to open X11 display");
            return;
        }

        unsigned int xButton = static_cast<unsigned int>(button) + 1;
        XTestFakeButtonEvent(display, xButton, press, 0);
        XFlush(display);
        XCloseDisplay(display);
    }

    void PlatformMoveMouse(int x, int y, bool absolute)
    {
        Display* display = XOpenDisplay(nullptr);
        if (!display)
        {
            LUMINA_LOG_ERROR("Failed to open X11 display");
            return;
        }

        if (absolute)
            XTestFakeMotionEvent(display, -1, x, y, 0);
        else
            XTestFakeRelativeMotionEvent(display, x, y, 0);

        XFlush(display);
        XCloseDisplay(display);
    }

    void PlatformScrollMouse(float xOffset, float yOffset)
    {
        Display* display = XOpenDisplay(nullptr);
        if (!display)
        {
            LUMINA_LOG_ERROR("Failed to open X11 display");
            return;
        }

        if (yOffset > 0)
        {
            XTestFakeButtonEvent(display, 4, True, 0);
            XTestFakeButtonEvent(display, 4, False, 0);
        }
        else if (yOffset < 0)
        {
            XTestFakeButtonEvent(display, 5, True, 0);
            XTestFakeButtonEvent(display, 5, False, 0);
        }

        if (xOffset > 0)
        {
            XTestFakeButtonEvent(display, 7, True, 0);
            XTestFakeButtonEvent(display, 7, False, 0);
        }
        else if (xOffset < 0)
        {
            XTestFakeButtonEvent(display, 6, True, 0);
            XTestFakeButtonEvent(display, 6, False, 0);
        }

        XFlush(display);
        XCloseDisplay(display);
    }
}

#endif