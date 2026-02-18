#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace lumina::core
{
    namespace input
    {
        enum class key_state : int
        {
            none = -1,
            released = 0,
            pressed = 1,
            held = 2
        };

        enum class key_code : int
        {
            unknown = 0,

            // Printable keys
            space = 32,
            apostrophe = 39,
            comma = 44,
            minus = 45,
            period = 46,
            slash = 47,

            num_0 = 48, num_1 = 49, num_2 = 50, num_3 = 51, num_4 = 52,
            num_5 = 53, num_6 = 54, num_7 = 55, num_8 = 56, num_9 = 57,

            semicolon = 59,
            equal = 61,

            a = 65, b = 66, c = 67, d = 68, e = 69, f = 70, g = 71, h = 72,
            i = 73, j = 74, k = 75, l = 76, m = 77, n = 78, o = 79, p = 80,
            q = 81, r = 82, s = 83, t = 84, u = 85, v = 86, w = 87, x = 88,
            y = 89, z = 90,

            left_bracket = 91,
            backslash = 92,
            right_bracket = 93,
            grave_accent = 96,
            world_1 = 161,
            world_2 = 162,

            // Function keys
            escape = 256,
            enter = 257,
            tab = 258,
            backspace = 259,
            insert = 260,
            del = 261,
            right = 262,
            left = 263,
            down = 264,
            up = 265,
            page_up = 266,
            page_down = 267,
            home = 268,
            end = 269,

            caps_lock = 280,
            scroll_lock = 281,
            num_lock = 282,
            print_screen = 283,
            pause = 284,

            f1 = 290, f2 = 291, f3 = 292, f4 = 293, f5 = 294, f6 = 295, f7 = 296,
            f8 = 297, f9 = 298, f10 = 299, f11 = 300, f12 = 301, f13 = 302, f14 = 303,
            f15 = 304, f16 = 305, f17 = 306, f18 = 307, f19 = 308, f20 = 309, f21 = 310,
            f22 = 311, f23 = 312, f24 = 313, f25 = 314,

            // Keypad
            kp_0 = 320, kp_1 = 321, kp_2 = 322, kp_3 = 323, kp_4 = 324,
            kp_5 = 325, kp_6 = 326, kp_7 = 327, kp_8 = 328, kp_9 = 329,
            kp_decimal = 330,
            kp_divide = 331,
            kp_multiply = 332,
            kp_subtract = 333,
            kp_add = 334,
            kp_enter = 335,
            kp_equal = 336,

            // Modifier keys
            left_shift = 340,
            left_control = 341,
            left_alt = 342,
            left_super = 343,
            right_shift = 344,
            right_control = 345,
            right_alt = 346,
            right_super = 347,

            menu = 348
        };

        enum class cursor_mode : int
        {
            normal = 0,
            hidden = 1,
            disabled = 2
        };

        enum class mouse_code : int
        {
            unknown = -1,
            button_0 = 0,
            button_1 = 1,
            button_2 = 2,
            button_3 = 3,
            button_4 = 4,
            button_5 = 5,
            button_6 = 6,
            button_7 = 7,

            left = button_0,
            right = button_1,
            middle = button_2,
            last = button_7
        };

        // Key state queries
        [[nodiscard]] bool is_key_pressed(key_code keycode);
        [[nodiscard]] bool is_mouse_button_pressed(mouse_code button);

        // Cursor operations
        void set_cursor_mode(cursor_mode mode);

        struct mouse_position { float x, y; };
        [[nodiscard]] mouse_position get_mouse_position();
        [[nodiscard]] inline float get_mouse_x() { return get_mouse_position().x; }
        [[nodiscard]] inline float get_mouse_y() { return get_mouse_position().y; }

        // Key code utilities
        [[nodiscard]] constexpr bool is_modifier_key(key_code keycode)
        {
            return keycode == key_code::left_shift || keycode == key_code::right_shift ||
                   keycode == key_code::left_control || keycode == key_code::right_control ||
                   keycode == key_code::left_alt || keycode == key_code::right_alt ||
                   keycode == key_code::left_super || keycode == key_code::right_super;
        }

        [[nodiscard]] constexpr bool is_printable_key(key_code keycode)
        {
            return (keycode >= key_code::space && keycode <= key_code::grave_accent) ||
                   (keycode >= key_code::world_1 && keycode <= key_code::world_2);
        }

        [[nodiscard]] constexpr bool is_function_key(key_code keycode)
        {
            return keycode >= key_code::f1 && keycode <= key_code::f25;
        }

        [[nodiscard]] constexpr bool is_arrow_key(key_code keycode)
        {
            return keycode == key_code::up || keycode == key_code::down ||
                   keycode == key_code::left || keycode == key_code::right;
        }

        [[nodiscard]] constexpr bool is_numpad_key(key_code keycode)
        {
            return keycode >= key_code::kp_0 && keycode <= key_code::kp_equal;
        }

        [[nodiscard]] constexpr bool is_letter_key(key_code keycode)
        {
            return keycode >= key_code::a && keycode <= key_code::z;
        }

        [[nodiscard]] constexpr bool is_number_key(key_code keycode)
        {
            return keycode >= key_code::num_0 && keycode <= key_code::num_9;
        }

        [[nodiscard]] constexpr std::string_view key_state_to_string(key_state state)
        {
            switch (state)
            {
            case key_state::none: return "none";
            case key_state::pressed: return "pressed";
            case key_state::held: return "held";
            case key_state::released: return "released";
            default: return "unknown";
            }
        }

        [[nodiscard]] constexpr std::string_view key_code_to_string(key_code keycode)
        {
            switch (keycode)
            {
            case key_code::unknown: return "unknown";
            case key_code::space: return "space";
            case key_code::apostrophe: return "apostrophe";
            case key_code::comma: return "comma";
            case key_code::minus: return "minus";
            case key_code::period: return "period";
            case key_code::slash: return "slash";
            case key_code::num_0: return "0";
            case key_code::num_1: return "1";
            case key_code::num_2: return "2";
            case key_code::num_3: return "3";
            case key_code::num_4: return "4";
            case key_code::num_5: return "5";
            case key_code::num_6: return "6";
            case key_code::num_7: return "7";
            case key_code::num_8: return "8";
            case key_code::num_9: return "9";
            case key_code::semicolon: return "semicolon";
            case key_code::equal: return "equal";
            case key_code::a: return "a";
            case key_code::b: return "b";
            case key_code::c: return "c";
            case key_code::d: return "d";
            case key_code::e: return "e";
            case key_code::f: return "f";
            case key_code::g: return "g";
            case key_code::h: return "h";
            case key_code::i: return "i";
            case key_code::j: return "j";
            case key_code::k: return "k";
            case key_code::l: return "l";
            case key_code::m: return "m";
            case key_code::n: return "n";
            case key_code::o: return "o";
            case key_code::p: return "p";
            case key_code::q: return "q";
            case key_code::r: return "r";
            case key_code::s: return "s";
            case key_code::t: return "t";
            case key_code::u: return "u";
            case key_code::v: return "v";
            case key_code::w: return "w";
            case key_code::x: return "x";
            case key_code::y: return "y";
            case key_code::z: return "z";
            case key_code::left_bracket: return "left_bracket";
            case key_code::backslash: return "backslash";
            case key_code::right_bracket: return "right_bracket";
            case key_code::grave_accent: return "grave_accent";
            case key_code::world_1: return "world_1";
            case key_code::world_2: return "world_2";
            case key_code::escape: return "escape";
            case key_code::enter: return "enter";
            case key_code::tab: return "tab";
            case key_code::backspace: return "backspace";
            case key_code::insert: return "insert";
            case key_code::del: return "delete";
            case key_code::right: return "right";
            case key_code::left: return "left";
            case key_code::down: return "down";
            case key_code::up: return "up";
            case key_code::page_up: return "page_up";
            case key_code::page_down: return "page_down";
            case key_code::home: return "home";
            case key_code::end: return "end";
            case key_code::caps_lock: return "caps_lock";
            case key_code::scroll_lock: return "scroll_lock";
            case key_code::num_lock: return "num_lock";
            case key_code::print_screen: return "print_screen";
            case key_code::pause: return "pause";
            case key_code::f1: return "f1";
            case key_code::f2: return "f2";
            case key_code::f3: return "f3";
            case key_code::f4: return "f4";
            case key_code::f5: return "f5";
            case key_code::f6: return "f6";
            case key_code::f7: return "f7";
            case key_code::f8: return "f8";
            case key_code::f9: return "f9";
            case key_code::f10: return "f10";
            case key_code::f11: return "f11";
            case key_code::f12: return "f12";
            case key_code::f13: return "f13";
            case key_code::f14: return "f14";
            case key_code::f15: return "f15";
            case key_code::f16: return "f16";
            case key_code::f17: return "f17";
            case key_code::f18: return "f18";
            case key_code::f19: return "f19";
            case key_code::f20: return "f20";
            case key_code::f21: return "f21";
            case key_code::f22: return "f22";
            case key_code::f23: return "f23";
            case key_code::f24: return "f24";
            case key_code::f25: return "f25";
            case key_code::kp_0: return "kp_0";
            case key_code::kp_1: return "kp_1";
            case key_code::kp_2: return "kp_2";
            case key_code::kp_3: return "kp_3";
            case key_code::kp_4: return "kp_4";
            case key_code::kp_5: return "kp_5";
            case key_code::kp_6: return "kp_6";
            case key_code::kp_7: return "kp_7";
            case key_code::kp_8: return "kp_8";
            case key_code::kp_9: return "kp_9";
            case key_code::kp_decimal: return "kp_decimal";
            case key_code::kp_divide: return "kp_divide";
            case key_code::kp_multiply: return "kp_multiply";
            case key_code::kp_subtract: return "kp_subtract";
            case key_code::kp_add: return "kp_add";
            case key_code::kp_enter: return "kp_enter";
            case key_code::kp_equal: return "kp_equal";
            case key_code::left_shift: return "left_shift";
            case key_code::left_control: return "left_control";
            case key_code::left_alt: return "left_alt";
            case key_code::left_super: return "left_super";
            case key_code::right_shift: return "right_shift";
            case key_code::right_control: return "right_control";
            case key_code::right_alt: return "right_alt";
            case key_code::right_super: return "right_super";
            case key_code::menu: return "menu";
            default: return "unknown";
            }
        }

        [[nodiscard]] constexpr std::string_view mouse_code_to_string(mouse_code button)
        {
            switch (button)
            {
            case mouse_code::unknown: return "unknown";
            case mouse_code::button_0: return "left";
            case mouse_code::button_1: return "right";
            case mouse_code::button_2: return "middle";
            case mouse_code::button_3: return "button_3";
            case mouse_code::button_4: return "button_4";
            case mouse_code::button_5: return "button_5";
            case mouse_code::button_6: return "button_6";
            case mouse_code::button_7: return "button_7";
            default: return "unknown";
            }
        }

        [[nodiscard]] constexpr std::string_view cursor_mode_to_string(cursor_mode mode)
        {
            switch (mode)
            {
            case cursor_mode::normal: return "normal";
            case cursor_mode::hidden: return "hidden";
            case cursor_mode::disabled: return "disabled";
            default: return "unknown";
            }
        }
    }
}
