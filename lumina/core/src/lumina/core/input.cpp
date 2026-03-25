#include "Input.h"
#include "Application.h"
#include "Assert.h"

#include <GLFW/glfw3.h>

namespace Lumina::Input
{
	bool IsKeyPressed(KeyCode keycode)
	{
		GLFWwindow* window = Application::Get().GetWindowHandle();
		LUMINA_ASSERT(window, "IsKeyPressed: window is null");

		int state = glfwGetKey(window, static_cast<int>(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool IsMouseButtonPressed(MouseCode button)
	{
		GLFWwindow* window = Application::Get().GetWindowHandle();
		LUMINA_ASSERT(window, "IsMouseButtonPressed: window is null");

		int state = glfwGetMouseButton(window, static_cast<int>(button));
		return state == GLFW_PRESS;
	}

	void SetCursorMode(CursorMode mode)
	{
		GLFWwindow* window = Application::Get().GetWindowHandle();
		LUMINA_ASSERT(window, "SetCursorMode: window is null");

		int glfwMode = GLFW_CURSOR_NORMAL;
		switch (mode)
		{
		case CursorMode::Normal:   glfwMode = GLFW_CURSOR_NORMAL; break;
		case CursorMode::Hidden:   glfwMode = GLFW_CURSOR_HIDDEN; break;
		case CursorMode::Disabled: glfwMode = GLFW_CURSOR_DISABLED; break;
		}

		glfwSetInputMode(window, GLFW_CURSOR, glfwMode);
	}

	MousePosition GetMousePosition()
	{
		GLFWwindow* window = Application::Get().GetWindowHandle();
		LUMINA_ASSERT(window, "GetMousePosition: window is null");

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { static_cast<float>(xpos), static_cast<float>(ypos) };
	}
}
