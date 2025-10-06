#pragma once

#include "../Core/Event.h"
#include <sstream>

namespace Lumina
{
    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height)
            : m_Width(width), m_Height(height) {
        }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << " x " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        uint32_t m_Width;
        uint32_t m_Height;
    };

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowFocusEvent : public Event
    {
    public:
        WindowFocusEvent() = default;

        EVENT_CLASS_TYPE(WindowFocus)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowLostFocusEvent : public Event
    {
    public:
        WindowLostFocusEvent() = default;

        EVENT_CLASS_TYPE(WindowLostFocus)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowMovedEvent : public Event
    {
    public:
        WindowMovedEvent(int x, int y)
            : m_X(x), m_Y(y) {
        }

        int GetX() const { return m_X; }
        int GetY() const { return m_Y; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowMovedEvent: (" << m_X << ", " << m_Y << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowMoved)
            EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:
        int m_X;
        int m_Y;
    };
}