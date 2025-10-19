#include "GlobalInput.h"
#include "Lumina/Core/Application.h"

#include "Lumina/Events/GlobalKeyEvent.h"
#include "Lumina/Events/GlobalMouseEvent.h"

#ifdef _WIN32
#include "Platform/WindowsGlobalInput.h"
#endif

namespace Lumina
{
    std::unique_ptr<GlobalInput> GlobalInput::Create()
    {
#ifdef _WIN32
        return std::make_unique<WindowsGlobalInput>();
#else
        return nullptr;
#endif
    }

    void GlobalInput::SetKeyCallback(GlobalKeyCallback callback)
    {
        m_KeyCallback = callback;
    }

    void GlobalInput::SetMouseButtonCallback(GlobalMouseButtonCallback callback)
    {
        m_MouseButtonCallback = callback;
    }

    void GlobalInput::SetMouseMoveCallback(GlobalMouseMoveCallback callback)
    {
        m_MouseMoveCallback = callback;
    }

    void GlobalInput::SetMouseScrollCallback(GlobalMouseScrollCallback callback)
    {
        m_ScrollCallback = callback;
    }

    void GlobalInput::SetPostEventsToApplication(bool enable)
    {
        m_PostEvents = enable;
    }

    void GlobalInput::PostGlobalKeyEvent(KeyCode key, bool pressed)
    {
        if (!m_PostEvents)
            return;

        if (pressed)
        {
            auto event = std::make_unique<GlobalKeyPressedEvent>(key);
            Application::GetInstance().QueueEvent(std::move(event));
        }
        else
        {
            auto event = std::make_unique<GlobalKeyReleasedEvent>(key);
            Application::GetInstance().QueueEvent(std::move(event));
        }
    }

    void GlobalInput::PostGlobalMouseButtonEvent(MouseCode button, bool pressed, int x, int y)
    {
        if (!m_PostEvents)
            return;

        if (pressed)
        {
            auto event = std::make_unique<GlobalMouseButtonPressedEvent>(button, x, y);
            Application::GetInstance().QueueEvent(std::move(event));
        }
        else
        {
            auto event = std::make_unique<GlobalMouseButtonReleasedEvent>(button, x, y);
            Application::GetInstance().QueueEvent(std::move(event));
        }
    }

    void GlobalInput::PostGlobalMouseMoveEvent(int x, int y)
    {
        if (!m_PostEvents)
            return;

        auto event = std::make_unique<GlobalMouseMovedEvent>(x, y);
        Application::GetInstance().QueueEvent(std::move(event));
    }

    void GlobalInput::PostGlobalMouseScrollEvent(int dx, int dy)
    {
        if (!m_PostEvents)
            return;

        auto event = std::make_unique<GlobalMouseScrolledEvent>(dx, dy);
        Application::GetInstance().QueueEvent(std::move(event));
    }
}