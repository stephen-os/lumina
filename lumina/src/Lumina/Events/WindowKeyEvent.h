#pragma once

#include "Event.h"

#include <Lumina/Core/KeyCodes.h>
#include <Lumina/Core/Input.h>

#include <sstream>

namespace Lumina
{
    class WindowKeyEvent : public Event
    {
    public:
        KeyCode GetKeyCode() const { return m_KeyCode; }
        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        WindowKeyEvent(KeyCode keycode) : m_KeyCode(keycode) {}
        KeyCode m_KeyCode;
    };

    class WindowKeyPressedEvent : public WindowKeyEvent
    {
    public:
        WindowKeyPressedEvent(KeyCode keycode, bool repeat = false) : WindowKeyEvent(keycode), m_Repeat(repeat) {}

        bool IsRepeat() const { return m_Repeat; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowKeyPressedEvent: " << Input::KeyCodeToString(m_KeyCode)
                << " (repeat = " << (m_Repeat ? "true" : "false") << ")";
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowKeyPressed)
    private:
        bool m_Repeat;
    };

    class WindowKeyReleasedEvent : public WindowKeyEvent
    {
    public:
        WindowKeyReleasedEvent(KeyCode keycode) : WindowKeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowKeyReleasedEvent: " << Input::KeyCodeToString(m_KeyCode);
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowKeyReleased)
    };

    class WindowKeyTypedEvent : public WindowKeyEvent
    {
    public:
        WindowKeyTypedEvent(KeyCode keycode) : WindowKeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowKeyTypedEvent: " << Input::KeyCodeToString(m_KeyCode);
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowKeyTyped)
    };
}