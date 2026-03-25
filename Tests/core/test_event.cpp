#include <catch2/catch_all.hpp>
#include <Lumina/Core/event.h>

using namespace lumina::core;

TEST_CASE("Window resize event stores dimensions", "[event]")
{
    window_resize_event event(1920, 1080);

    REQUIRE(event.get_width() == 1920);
    REQUIRE(event.get_height() == 1080);
    REQUIRE(event.get_type() == event_type::window_resize);
    REQUIRE(event.get_name() == "window_resize");
}

TEST_CASE("Window close event has correct type", "[event]")
{
    window_close_event event;

    REQUIRE(event.get_type() == event_type::window_close);
    REQUIRE(event.get_name() == "window_close");
}

TEST_CASE("Key pressed event stores key code and repeat", "[event]")
{
    key_pressed_event event(input::key_code::a, false);

    REQUIRE(event.get_key() == input::key_code::a);
    REQUIRE(event.is_repeat() == false);
    REQUIRE(event.get_type() == event_type::key_pressed);

    key_pressed_event repeat_event(input::key_code::space, true);
    REQUIRE(repeat_event.is_repeat() == true);
}

TEST_CASE("Key released event stores key code", "[event]")
{
    key_released_event event(input::key_code::escape);

    REQUIRE(event.get_key() == input::key_code::escape);
    REQUIRE(event.get_type() == event_type::key_released);
}

TEST_CASE("Mouse button events store button code", "[event]")
{
    mouse_button_pressed_event pressed(input::mouse_code::left);
    REQUIRE(pressed.get_button() == input::mouse_code::left);
    REQUIRE(pressed.get_type() == event_type::mouse_button_pressed);

    mouse_button_released_event released(input::mouse_code::right);
    REQUIRE(released.get_button() == input::mouse_code::right);
    REQUIRE(released.get_type() == event_type::mouse_button_released);
}

TEST_CASE("Mouse move event stores position", "[event]")
{
    mouse_moved_event event(100.5f, 200.75f);

    REQUIRE(event.get_x() == 100.5f);
    REQUIRE(event.get_y() == 200.75f);
    REQUIRE(event.get_type() == event_type::mouse_moved);
}

TEST_CASE("Mouse scroll event stores offsets", "[event]")
{
    mouse_scrolled_event event(0.0f, 3.0f);

    REQUIRE(event.get_x_offset() == 0.0f);
    REQUIRE(event.get_y_offset() == 3.0f);
    REQUIRE(event.get_type() == event_type::mouse_scrolled);
}

TEST_CASE("Event dispatcher dispatches to correct handler", "[event]")
{
    window_resize_event resize_event(800, 600);
    bool resize_handled = false;
    bool close_handled = false;

    event_dispatcher dispatcher(resize_event);

    dispatcher.dispatch<window_resize_event>([&](window_resize_event& e) {
        resize_handled = true;
        return true;
    });

    dispatcher.dispatch<window_close_event>([&](window_close_event& e) {
        close_handled = true;
        return true;
    });

    REQUIRE(resize_handled == true);
    REQUIRE(close_handled == false);
    REQUIRE(resize_event.is_handled() == true);
}

TEST_CASE("Event handled flag works correctly", "[event]")
{
    key_pressed_event event(input::key_code::enter, false);

    REQUIRE(event.is_handled() == false);

    event_dispatcher dispatcher(event);
    dispatcher.dispatch<key_pressed_event>([](key_pressed_event& e) {
        return true;
    });

    REQUIRE(event.is_handled() == true);
}
