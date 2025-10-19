#include "GlobalInputCapture.h"
#include "Lumina/Core/Application.h"

#include "Lumina/Events/GlobalKeyEvent.h"
#include "Lumina/Events/GlobalMouseEvent.h"

#ifdef _WIN32
#include "Platform/WindowsGlobalInputCapture.h"
#endif

namespace Lumina
{
    std::unique_ptr<GlobalInputCapture> GlobalInputCapture::Create()
    {
#ifdef _WIN32
        return std::make_unique<WindowsGlobalInputCapture>();
#else
        return nullptr;
#endif
    }

    void GlobalInputCapture::SetKeyCallback(GlobalKeyCallback callback)
    {
        m_KeyCallback = callback;
    }

    void GlobalInputCapture::SetMouseButtonCallback(GlobalMouseButtonCallback callback)
    {
        m_MouseButtonCallback = callback;
    }

    void GlobalInputCapture::SetMouseMoveCallback(GlobalMouseMoveCallback callback)
    {
        m_MouseMoveCallback = callback;
    }

    void GlobalInputCapture::SetMouseScrollCallback(GlobalMouseScrollCallback callback)
    {
        m_ScrollCallback = callback;
    }

    void GlobalInputCapture::SetPostEventsToApplication(bool enable)
    {
        m_PostEvents = enable;
    }

    void GlobalInputCapture::PostGlobalKeyEvent(KeyCode key, bool pressed)
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

    void GlobalInputCapture::PostGlobalMouseButtonEvent(MouseCode button, bool pressed, int x, int y)
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

    void GlobalInputCapture::PostGlobalMouseMoveEvent(int x, int y)
    {
        if (!m_PostEvents)
            return;

        auto event = std::make_unique<GlobalMouseMovedEvent>(x, y);
        Application::GetInstance().QueueEvent(std::move(event));
    }

    void GlobalInputCapture::PostGlobalMouseScrollEvent(int dx, int dy)
    {
        if (!m_PostEvents)
            return;

        auto event = std::make_unique<GlobalMouseScrolledEvent>(dx, dy);
        Application::GetInstance().QueueEvent(std::move(event));
    }
}