#include "input.h"
#include "application.h"
#include "assert.h"

#include <GLFW/glfw3.h>

namespace lumina::core::input
{
    bool is_key_pressed(key_code keycode)
    {
        GLFWwindow* window = application::get().get_window_handle();
        LUMINA_ASSERT(window, "is_key_pressed: window is null");

        int state = glfwGetKey(window, static_cast<int>(keycode));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool is_mouse_button_pressed(mouse_code button)
    {
        GLFWwindow* window = application::get().get_window_handle();
        LUMINA_ASSERT(window, "is_mouse_button_pressed: window is null");

        int state = glfwGetMouseButton(window, static_cast<int>(button));
        return state == GLFW_PRESS;
    }

    void set_cursor_mode(cursor_mode mode)
    {
        GLFWwindow* window = application::get().get_window_handle();
        LUMINA_ASSERT(window, "set_cursor_mode: window is null");

        int glfw_mode = GLFW_CURSOR_NORMAL;
        switch (mode)
        {
        case cursor_mode::normal:   glfw_mode = GLFW_CURSOR_NORMAL; break;
        case cursor_mode::hidden:   glfw_mode = GLFW_CURSOR_HIDDEN; break;
        case cursor_mode::disabled: glfw_mode = GLFW_CURSOR_DISABLED; break;
        }

        glfwSetInputMode(window, GLFW_CURSOR, glfw_mode);
    }

    mouse_position get_mouse_position()
    {
        GLFWwindow* window = application::get().get_window_handle();
        LUMINA_ASSERT(window, "get_mouse_position: window is null");

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return { static_cast<float>(xpos), static_cast<float>(ypos) };
    }
}
