#include <catch2/catch_all.hpp>
#include <Lumina/Core/input.h>

using namespace lumina::core::input;

TEST_CASE("is_modifier_key identifies modifier keys", "[input]")
{
    REQUIRE(is_modifier_key(key_code::left_shift) == true);
    REQUIRE(is_modifier_key(key_code::right_shift) == true);
    REQUIRE(is_modifier_key(key_code::left_control) == true);
    REQUIRE(is_modifier_key(key_code::right_control) == true);
    REQUIRE(is_modifier_key(key_code::left_alt) == true);
    REQUIRE(is_modifier_key(key_code::right_alt) == true);
    REQUIRE(is_modifier_key(key_code::left_super) == true);
    REQUIRE(is_modifier_key(key_code::right_super) == true);

    REQUIRE(is_modifier_key(key_code::a) == false);
    REQUIRE(is_modifier_key(key_code::space) == false);
    REQUIRE(is_modifier_key(key_code::enter) == false);
}

TEST_CASE("is_printable_key identifies printable keys", "[input]")
{
    REQUIRE(is_printable_key(key_code::a) == true);
    REQUIRE(is_printable_key(key_code::z) == true);
    REQUIRE(is_printable_key(key_code::num_0) == true);
    REQUIRE(is_printable_key(key_code::space) == true);
    REQUIRE(is_printable_key(key_code::comma) == true);

    REQUIRE(is_printable_key(key_code::escape) == false);
    REQUIRE(is_printable_key(key_code::enter) == false);
    REQUIRE(is_printable_key(key_code::f1) == false);
}

TEST_CASE("is_function_key identifies function keys", "[input]")
{
    REQUIRE(is_function_key(key_code::f1) == true);
    REQUIRE(is_function_key(key_code::f12) == true);
    REQUIRE(is_function_key(key_code::f25) == true);

    REQUIRE(is_function_key(key_code::a) == false);
    REQUIRE(is_function_key(key_code::escape) == false);
}

TEST_CASE("is_arrow_key identifies arrow keys", "[input]")
{
    REQUIRE(is_arrow_key(key_code::up) == true);
    REQUIRE(is_arrow_key(key_code::down) == true);
    REQUIRE(is_arrow_key(key_code::left) == true);
    REQUIRE(is_arrow_key(key_code::right) == true);

    REQUIRE(is_arrow_key(key_code::a) == false);
    REQUIRE(is_arrow_key(key_code::w) == false);
}

TEST_CASE("is_numpad_key identifies numpad keys", "[input]")
{
    REQUIRE(is_numpad_key(key_code::kp_0) == true);
    REQUIRE(is_numpad_key(key_code::kp_9) == true);
    REQUIRE(is_numpad_key(key_code::kp_add) == true);
    REQUIRE(is_numpad_key(key_code::kp_enter) == true);
    REQUIRE(is_numpad_key(key_code::kp_equal) == true);

    REQUIRE(is_numpad_key(key_code::num_0) == false);
    REQUIRE(is_numpad_key(key_code::enter) == false);
}

TEST_CASE("is_letter_key identifies letter keys", "[input]")
{
    REQUIRE(is_letter_key(key_code::a) == true);
    REQUIRE(is_letter_key(key_code::z) == true);
    REQUIRE(is_letter_key(key_code::m) == true);

    REQUIRE(is_letter_key(key_code::num_0) == false);
    REQUIRE(is_letter_key(key_code::space) == false);
}

TEST_CASE("is_number_key identifies number keys", "[input]")
{
    REQUIRE(is_number_key(key_code::num_0) == true);
    REQUIRE(is_number_key(key_code::num_9) == true);
    REQUIRE(is_number_key(key_code::num_5) == true);

    REQUIRE(is_number_key(key_code::kp_0) == false);
    REQUIRE(is_number_key(key_code::a) == false);
}

TEST_CASE("key_state_to_string converts states", "[input]")
{
    REQUIRE(key_state_to_string(key_state::none) == "none");
    REQUIRE(key_state_to_string(key_state::pressed) == "pressed");
    REQUIRE(key_state_to_string(key_state::held) == "held");
    REQUIRE(key_state_to_string(key_state::released) == "released");
}

TEST_CASE("key_code_to_string converts key codes", "[input]")
{
    REQUIRE(key_code_to_string(key_code::a) == "a");
    REQUIRE(key_code_to_string(key_code::space) == "space");
    REQUIRE(key_code_to_string(key_code::escape) == "escape");
    REQUIRE(key_code_to_string(key_code::enter) == "enter");
    REQUIRE(key_code_to_string(key_code::f1) == "f1");
    REQUIRE(key_code_to_string(key_code::left_shift) == "left_shift");
    REQUIRE(key_code_to_string(key_code::unknown) == "unknown");
}

TEST_CASE("mouse_code_to_string converts mouse codes", "[input]")
{
    REQUIRE(mouse_code_to_string(mouse_code::left) == "left");
    REQUIRE(mouse_code_to_string(mouse_code::right) == "right");
    REQUIRE(mouse_code_to_string(mouse_code::middle) == "middle");
    REQUIRE(mouse_code_to_string(mouse_code::button_3) == "button_3");
    REQUIRE(mouse_code_to_string(mouse_code::unknown) == "unknown");
}

TEST_CASE("cursor_mode_to_string converts cursor modes", "[input]")
{
    REQUIRE(cursor_mode_to_string(cursor_mode::normal) == "normal");
    REQUIRE(cursor_mode_to_string(cursor_mode::hidden) == "hidden");
    REQUIRE(cursor_mode_to_string(cursor_mode::disabled) == "disabled");
}

TEST_CASE("Key code enum values match GLFW", "[input]")
{
    // Verify some key codes have correct values (GLFW compatibility)
    REQUIRE(static_cast<int>(key_code::space) == 32);
    REQUIRE(static_cast<int>(key_code::a) == 65);
    REQUIRE(static_cast<int>(key_code::escape) == 256);
    REQUIRE(static_cast<int>(key_code::enter) == 257);
    REQUIRE(static_cast<int>(key_code::f1) == 290);
}

TEST_CASE("Mouse code aliases work correctly", "[input]")
{
    REQUIRE(mouse_code::left == mouse_code::button_0);
    REQUIRE(mouse_code::right == mouse_code::button_1);
    REQUIRE(mouse_code::middle == mouse_code::button_2);
    REQUIRE(mouse_code::last == mouse_code::button_7);
}
