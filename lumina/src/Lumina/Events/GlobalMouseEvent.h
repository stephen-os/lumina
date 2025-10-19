#pragma once

#include "Event.h"

#include "Lumina/Core/KeyCodes.h"

#include <sstream>

namespace Lumina
{
    class GlobalMouseMovedEvent : public Event
    {
    public:
        GlobalMouseMovedEvent(int x, int y) : m_X(x), m_Y(y) {}

        int GetX() const { return m_X; }
        int GetY() const { return m_Y; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "GlobalMouseMovedEvent: (" << m_X << ", " << m_Y << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(GlobalMouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        int m_X, m_Y;
    };

    class GlobalMouseScrolledEvent : public Event
    {
    public:
        GlobalMouseScrolledEvent(int dx, int dy) : m_DX(dx), m_DY(dy) {}

        int GetDX() const { return m_DX; }
        int GetDY() const { return m_DY; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "GlobalMouseScrolledEvent: (" << m_DX << ", " << m_DY << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(GlobalMouseScrolled)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

    private:
        int m_DX, m_DY;
    };

    class GlobalMouseButtonEvent : public Event
    {
    public:
        MouseCode GetMouseButton() const { return m_Button; }
        int GetX() const { return m_X; }
        int GetY() const { return m_Y; }
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

    protected:
        GlobalMouseButtonEvent(MouseCode button, int x, int y)
            : m_Button(button), m_X(x), m_Y(y) {
        }

        MouseCode m_Button;
        int m_X, m_Y;
    };

    class GlobalMouseButtonPressedEvent : public GlobalMouseButtonEvent
    {
    public:
        GlobalMouseButtonPressedEvent(MouseCode button, int x, int y)
            : GlobalMouseButtonEvent(button, x, y) {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "GlobalMouseButtonPressedEvent: ";
            switch (m_Button)
            {
            case MouseCode::Left:   ss << "Left"; break;
            case MouseCode::Right:  ss << "Right"; break;
            case MouseCode::Middle: ss << "Middle"; break;
            default: ss << "Button" << static_cast<int>(m_Button); break;
            }
            ss << " at (" << m_X << ", " << m_Y << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(GlobalMouseButtonPressed)
    };

    class GlobalMouseButtonReleasedEvent : public GlobalMouseButtonEvent
    {
    public:
        GlobalMouseButtonReleasedEvent(MouseCode button, int x, int y)
            : GlobalMouseButtonEvent(button, x, y) {
        }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "GlobalMouseButtonReleasedEvent: ";
            switch (m_Button)
            {
            case MouseCode::Left:   ss << "Left"; break;
            case MouseCode::Right:  ss << "Right"; break;
            case MouseCode::Middle: ss << "Middle"; break;
            default: ss << "Button" << static_cast<int>(m_Button); break;
            }
            ss << " at (" << m_X << ", " << m_Y << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(GlobalMouseButtonReleased)
    };
}