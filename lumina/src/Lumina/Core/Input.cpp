#include "Input.h"

#include <algorithm>

#include <GLFW/glfw3.h>

#include "Application.h"
#include"Assert.h"

namespace Lumina::Core::Input
{
    bool KeyCombo::IsKeyComboPressed() const
    {
        if (IsEmpty())
			return false;

        for (const auto& key : m_Keys)
        {
            if (!IsKeyPressed(key))
                return false;
        }

        return true;
	}

    bool KeyCombo::AddKeyCode(KeyCode key)
    {
        if (IsKeyCodeInCombo(key))
            return false;

        m_Keys.push_back(key);
        return true; 
	}

    bool KeyCombo::RemoveKeyCode(KeyCode key)
    {
        auto it = std::find(m_Keys.begin(), m_Keys.end(), key);
        if (it != m_Keys.end())
        {
            m_Keys.erase(it);
            return true;
        }

        return false;
	}

    bool KeyCombo::IsKeyCodeInCombo(KeyCode key) const
    {
        return std::find(m_Keys.begin(), m_Keys.end(), key) != m_Keys.end();
    }

    bool IsKeyPressed(KeyCode keycode)
    {
        GLFWwindow* window = Application::GetInstance().GetWindowHandle();
        LUMINA_ASSERT(window, "Core::IsKeyPressed: Window is null.");

        int state = glfwGetKey(window, static_cast<int>(keycode));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool IsMouseButtonPressed(MouseCode mousecode)
    {
        GLFWwindow* window = Application::GetInstance().GetWindowHandle();
        LUMINA_ASSERT(window, "Core::IsMouseButtonPressed: Window is null.");

        int state = glfwGetMouseButton(window, static_cast<int>(mousecode));
        return state == GLFW_PRESS;
    }

    void SetCursorMode(CursorMode mode)
    {
        GLFWwindow* window = Application::GetInstance().GetWindowHandle();
        LUMINA_ASSERT(window, "Core::SetCursorMode: Window is null.");

        int glfwMode = GLFW_CURSOR_NORMAL;

        switch (mode)
        {
        case CursorMode::Normal:
            glfwMode = GLFW_CURSOR_NORMAL;
            break;
        case CursorMode::Hidden:
            glfwMode = GLFW_CURSOR_HIDDEN;
            break;
        case CursorMode::Disabled:
            glfwMode = GLFW_CURSOR_DISABLED;
            break;
        }

        glfwSetInputMode(window, GLFW_CURSOR, glfwMode);
    }

    glm::vec2 GetMousePosition()
    {
        GLFWwindow* window = Application::GetInstance().GetWindowHandle();
        LUMINA_ASSERT(window, "Core::GetMousePosition: Window is null.");

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return { static_cast<float>(xpos), static_cast<float>(ypos) };
    }
}