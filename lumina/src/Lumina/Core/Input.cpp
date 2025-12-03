#include "Input.h"

#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <atomic>

#include <GLFW/glfw3.h>

#include "Application.h"
#include"Assert.h"

namespace Lumina::Core::Input
{
    struct SimulationQueue
    {
        std::queue<std::function<void()>> actions;
        std::mutex mutex;
        std::thread thread;
        std::atomic<bool> running{ false };
        std::atomic<bool> shouldStop{ false };
    };

    static SimulationQueue s_SimulationQueue;

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

    void PressKeyGlobal(KeyCode keycode)
    {
        PlatformPressKey(keycode, true);
    }

    void ReleaseKeyGlobal(KeyCode keycode)
    {
        PlatformPressKey(keycode, false);
    }

    void TapKeyGlobal(KeyCode keycode, Duration holdDuration)
    {
        PressKeyGlobal(keycode);
        if (holdDuration.Milliseconds() > 0.0f)
        {
            std::this_thread::sleep_for(holdDuration.ToChronoMilliseconds());
        }
        ReleaseKeyGlobal(keycode);
    }

    void TypeTextGlobal(std::string_view text, Duration delayBetweenKeys)
    {
        for (char c : text)
        {
            KeyCode key;

            if (c >= 'a' && c <= 'z')
                key = static_cast<KeyCode>(c - 32);
            else if (c >= 'A' && c <= 'Z')
                key = static_cast<KeyCode>(c);
            else if (c >= '0' && c <= '9')
                key = static_cast<KeyCode>(c);
            else if (c == ' ')
                key = KeyCode::Space;
            else
                continue;

            TapKeyGlobal(key);

            if (delayBetweenKeys.Milliseconds() > 0.0f)
            {
                std::this_thread::sleep_for(delayBetweenKeys.ToChronoMilliseconds());
            }
        }
    }

    void PressMouseButtonGlobal(MouseCode button)
    {
        PlatformPressMouseButton(button, true);
    }

    void ReleaseMouseButtonGlobal(MouseCode button)
    {
        PlatformPressMouseButton(button, false);
    }

    void ClickMouseButtonGlobal(MouseCode button)
    {
        PressMouseButtonGlobal(button);
        ReleaseMouseButtonGlobal(button);
    }

    void MoveMouseGlobal(int x, int y)
    {
        PlatformMoveMouse(x, y, true);
    }

    void MoveMouseRelativeGlobal(int deltaX, int deltaY)
    {
        PlatformMoveMouse(deltaX, deltaY, false);
    }

    void ScrollMouseGlobal(float xOffset, float yOffset)
    {
        PlatformScrollMouse(xOffset, yOffset);
    }

    void QueueKeyPress(KeyCode keycode)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        s_SimulationQueue.actions.push([keycode]() {
            PressKeyGlobal(keycode);
            });
    }

    void QueueKeyRelease(KeyCode keycode)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        s_SimulationQueue.actions.push([keycode]() {
            ReleaseKeyGlobal(keycode);
            });
    }

    void QueueKeyTap(KeyCode keycode, Duration holdDuration)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        s_SimulationQueue.actions.push([keycode, holdDuration]() {
            TapKeyGlobal(keycode, holdDuration);
            });
    }

    void QueueTypeText(std::string_view text, Duration delayBetweenKeys)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        std::string textCopy(text);
        s_SimulationQueue.actions.push([textCopy, delayBetweenKeys]() {
            TypeTextGlobal(textCopy, delayBetweenKeys);
            });
    }

    void QueueMouseButtonPress(MouseCode button)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        s_SimulationQueue.actions.push([button]() {
            PressMouseButtonGlobal(button);
            });
    }

    void QueueMouseButtonRelease(MouseCode button)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        s_SimulationQueue.actions.push([button]() {
            ReleaseMouseButtonGlobal(button);
            });
    }

    void QueueMouseButtonClick(MouseCode button)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        s_SimulationQueue.actions.push([button]() {
            ClickMouseButtonGlobal(button);
            });
    }

    void QueueMouseMove(int x, int y)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        s_SimulationQueue.actions.push([x, y]() {
            MoveMouseGlobal(x, y);
            });
    }

    void QueueMouseMoveRelative(int deltaX, int deltaY)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        s_SimulationQueue.actions.push([deltaX, deltaY]() {
            MoveMouseRelativeGlobal(deltaX, deltaY);
            });
    }

    void QueueMouseScroll(float xOffset, float yOffset)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        s_SimulationQueue.actions.push([xOffset, yOffset]() {
            ScrollMouseGlobal(xOffset, yOffset);
            });
    }

    void QueueDelay(Duration delay)
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        s_SimulationQueue.actions.push([delay]() {
            std::this_thread::sleep_for(delay.ToChronoMilliseconds());
            });
    }

    void StartSimulationQueue()
    {
        if (s_SimulationQueue.running)
        {
            LUMINA_LOG_WARN("Simulation queue already running");
            return;
        }

        s_SimulationQueue.running = true;
        s_SimulationQueue.shouldStop = false;

        s_SimulationQueue.thread = std::thread([]() {
            LUMINA_LOG_INFO("Simulation queue thread started");

            while (s_SimulationQueue.running)
            {
                std::function<void()> action;

                {
                    std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
                    if (s_SimulationQueue.actions.empty())
                    {
                        s_SimulationQueue.running = false;
                        break;
                    }

                    action = std::move(s_SimulationQueue.actions.front());
                    s_SimulationQueue.actions.pop();
                }

                if (s_SimulationQueue.shouldStop)
                    break;

                action();
            }

            s_SimulationQueue.running = false;
            LUMINA_LOG_INFO("Simulation queue thread stopped");
            });
    }

    void StopSimulationQueue()
    {
        if (!s_SimulationQueue.running)
            return;

        s_SimulationQueue.shouldStop = true;

        if (s_SimulationQueue.thread.joinable())
        {
            s_SimulationQueue.thread.join();
        }

        LUMINA_LOG_INFO("Simulation queue stopped");
    }

    void ClearSimulationQueue()
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);

        std::queue<std::function<void()>> empty;
        std::swap(s_SimulationQueue.actions, empty);

        LUMINA_LOG_INFO("Simulation queue cleared");
    }

    void ShutdownSimulationQueue()
    {
        if (s_SimulationQueue.running || s_SimulationQueue.thread.joinable())
        {
            StopSimulationQueue();
            ClearSimulationQueue();
        }
    }

    bool IsSimulationQueueRunning()
    {
        return s_SimulationQueue.running;
    }

    size_t GetSimulationQueueSize()
    {
        std::lock_guard<std::mutex> lock(s_SimulationQueue.mutex);
        return s_SimulationQueue.actions.size();
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