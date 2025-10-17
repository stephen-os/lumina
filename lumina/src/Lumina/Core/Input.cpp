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

    std::string Input::KeyCodeToString(KeyCode keycode)
    {
        int code = static_cast<int>(keycode);

        // Printable characters
        if (code >= 32 && code <= 126)
            return std::string(1, (char)code);

        // Function keys
        if (keycode >= KeyCode::F1 && keycode <= KeyCode::F25)
        {
            int fNum = code - static_cast<int>(KeyCode::F1) + 1;
            return "F" + std::to_string(fNum);
        }

        // Keypad numbers
        if (keycode >= KeyCode::KP0 && keycode <= KeyCode::KP9)
        {
            int kpNum = code - static_cast<int>(KeyCode::KP0);
            return "KP" + std::to_string(kpNum);
        }

        // Special keys
        switch (keycode)
        {
        case KeyCode::Unknown: return "UNKNOWN";
        case KeyCode::Space: return "SPACE";
        case KeyCode::Apostrophe: return "APOSTROPHE";
        case KeyCode::Comma: return "COMMA";
        case KeyCode::Minus: return "MINUS";
        case KeyCode::Period: return "PERIOD";
        case KeyCode::Slash: return "SLASH";
        case KeyCode::Semicolon: return "SEMICOLON";
        case KeyCode::Equal: return "EQUAL";
        case KeyCode::LeftBracket: return "LEFT_BRACKET";
        case KeyCode::Backslash: return "BACKSLASH";
        case KeyCode::RightBracket: return "RIGHT_BRACKET";
        case KeyCode::GraveAccent: return "GRAVE_ACCENT";
        case KeyCode::Escape: return "ESCAPE";
        case KeyCode::Enter: return "ENTER";
        case KeyCode::Tab: return "TAB";
        case KeyCode::Backspace: return "BACKSPACE";
        case KeyCode::Insert: return "INSERT";
        case KeyCode::Delete: return "DELETE";
        case KeyCode::Right: return "RIGHT";
        case KeyCode::Left: return "LEFT";
        case KeyCode::Down: return "DOWN";
        case KeyCode::Up: return "UP";
        case KeyCode::PageUp: return "PAGE_UP";
        case KeyCode::PageDown: return "PAGE_DOWN";
        case KeyCode::Home: return "HOME";
        case KeyCode::End: return "END";
        case KeyCode::CapsLock: return "CAPS_LOCK";
        case KeyCode::ScrollLock: return "SCROLL_LOCK";
        case KeyCode::NumLock: return "NUM_LOCK";
        case KeyCode::PrintScreen: return "PRINT_SCREEN";
        case KeyCode::Pause: return "PAUSE";
        case KeyCode::KPDecimal: return "KP_DECIMAL";
        case KeyCode::KPDivide: return "KP_DIVIDE";
        case KeyCode::KPMultiply: return "KP_MULTIPLY";
        case KeyCode::KPSubtract: return "KP_SUBTRACT";
        case KeyCode::KPAdd: return "KP_ADD";
        case KeyCode::KPEnter: return "KP_ENTER";
        case KeyCode::KPEqual: return "KP_EQUAL";
        case KeyCode::LeftShift: return "LEFT_SHIFT";
        case KeyCode::LeftControl: return "LEFT_CONTROL";
        case KeyCode::LeftAlt: return "LEFT_ALT";
        case KeyCode::LeftSuper: return "LEFT_SUPER";
        case KeyCode::RightShift: return "RIGHT_SHIFT";
        case KeyCode::RightControl: return "RIGHT_CONTROL";
        case KeyCode::RightAlt: return "RIGHT_ALT";
        case KeyCode::RightSuper: return "RIGHT_SUPER";
        case KeyCode::Menu: return "MENU";
        default: return "UNKNOWN_" + std::to_string(code);
        }
    }
}
