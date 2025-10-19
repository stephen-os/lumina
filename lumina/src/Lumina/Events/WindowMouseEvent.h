#pragma once

#include "Event.h"

#include "Lumina/Core/KeyCodes.h"

#include <sstream>

namespace Lumina
{
    class WindowMouseMovedEvent : public Event
    {
    public:
        WindowMouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}
        float GetX() const { return m_MouseX; }
        float GetY() const { return m_MouseY; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowMouseMovedEvent: (" << m_MouseX << ", " << m_MouseY << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowMouseMoved)
            EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        float m_MouseX;
        float m_MouseY;
    };

    class WindowMouseScrolledEvent : public Event
    {
    public:
        WindowMouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

        float GetXOffset() const { return m_XOffset; }
        float GetYOffset() const { return m_YOffset; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowMouseScrolledEvent: (" << m_XOffset << ", " << m_YOffset << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowMouseScrolled)
            EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    private:
        float m_XOffset;
        float m_YOffset;
    };

    class WindowMouseButtonEvent : public Event
    {
    public:
        MouseCode GetMouseButton() const { return m_Button; }
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
    protected:
        WindowMouseButtonEvent(MouseCode button)
            : m_Button(button) {
        }
        MouseCode m_Button;
    };

    class WindowMouseButtonPressedEvent : public WindowMouseButtonEvent
    {
    public:
        WindowMouseButtonPressedEvent(MouseCode button) : WindowMouseButtonEvent(button) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowMouseButtonPressedEvent: ";
            switch (m_Button)
            {
            case MouseCode::Left:   ss << "Left"; break;
            case MouseCode::Right:  ss << "Right"; break;
            case MouseCode::Middle: ss << "Middle"; break;
            default: ss << "Button" << static_cast<int>(m_Button); break;
            }
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowMouseButtonPressed)
    };

    class WindowMouseButtonReleasedEvent : public WindowMouseButtonEvent
    {
    public:
        WindowMouseButtonReleasedEvent(MouseCode button) : WindowMouseButtonEvent(button) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowMouseButtonReleasedEvent: ";
            switch (m_Button)
            {
            case MouseCode::Left:   ss << "Left"; break;
            case MouseCode::Right:  ss << "Right"; break;
            case MouseCode::Middle: ss << "Middle"; break;
            default: ss << "Button" << static_cast<int>(m_Button); break;
            }
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowMouseButtonReleased)
    };
}