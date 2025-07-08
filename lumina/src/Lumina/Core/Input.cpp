#include "Input.h"

#include <GLFW/glfw3.h>

#include "Application.h"

#include "Assert.h"

namespace Lumina 
{
    bool Input::IsKeyPressed(KeyCode keycode)
    {
        GLFWwindow* window = Application::GetInstance().GetWindowHandle();
		LUMINA_ASSERT(window, "Window is null.");

        int state = glfwGetKey(window, (int)keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(MouseCode mousecode)
    {
        GLFWwindow* window = Application::GetInstance().GetWindowHandle();
        LUMINA_ASSERT(window, "Window is null.");

        int state = glfwGetMouseButton(window, (int)mousecode);
        return state == GLFW_PRESS;
    }

    std::pair<float, float> Input::GetMousePosition()
    {
        GLFWwindow* window = Application::GetInstance().GetWindowHandle();
        LUMINA_ASSERT(window, "Window is null.");

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        return { (float)xpos, (float)ypos };
    }

    float Input::GetMouseX()
    {
        auto [x, _] = GetMousePosition();
        return x;
    }

    float Input::GetMouseY()
    {
        auto [_, y] = GetMousePosition();
        return y;
    }

    void Input::SetCursorMode(CursorMode mode)
    {
        GLFWwindow* window = Application::GetInstance().GetWindowHandle();
        int glfwMode = GLFW_CURSOR_NORMAL;

        switch (mode)
        {
        case CursorMode::Normal:   glfwMode = GLFW_CURSOR_NORMAL; break;
        case CursorMode::Hidden:   glfwMode = GLFW_CURSOR_HIDDEN; break;
        case CursorMode::Disabled: glfwMode = GLFW_CURSOR_DISABLED; break;
        }

        glfwSetInputMode(window, GLFW_CURSOR, glfwMode);
    }
}
