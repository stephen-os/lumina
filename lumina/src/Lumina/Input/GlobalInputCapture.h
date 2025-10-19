#pragma once

#include "Lumina/Core/KeyCodes.h"

#include <functional>
#include <memory>

namespace Lumina
{
    class Event;

    using GlobalKeyCallback = std::function<void(KeyCode key, bool pressed)>;
    using GlobalMouseButtonCallback = std::function<void(MouseCode button, bool pressed, int x, int y)>;
    using GlobalMouseMoveCallback = std::function<void(int x, int y)>;
    using GlobalMouseScrollCallback = std::function<void(int dx, int dy)>;

    class GlobalInputCapture
    {
    public:
        virtual ~GlobalInputCapture() = default;

        virtual bool Start() = 0;
        virtual void Stop() = 0;
        virtual bool IsActive() const = 0;

        void SetKeyCallback(GlobalKeyCallback callback);
        void SetMouseButtonCallback(GlobalMouseButtonCallback callback);
        void SetMouseMoveCallback(GlobalMouseMoveCallback callback);
        void SetMouseScrollCallback(GlobalMouseScrollCallback callback);

        void SetPostEventsToApplication(bool enable);
        bool IsPostingEvents() const { return m_PostEvents; }

        static std::unique_ptr<GlobalInputCapture> Create();

    protected:
        GlobalKeyCallback m_KeyCallback;
        GlobalMouseButtonCallback m_MouseButtonCallback;
        GlobalMouseMoveCallback m_MouseMoveCallback;
        GlobalMouseScrollCallback m_ScrollCallback;

        bool m_PostEvents = false;

        void PostGlobalKeyEvent(KeyCode key, bool pressed);
        void PostGlobalMouseButtonEvent(MouseCode button, bool pressed, int x, int y);
        void PostGlobalMouseMoveEvent(int x, int y);
        void PostGlobalMouseScrollEvent(int dx, int dy);
    };
}