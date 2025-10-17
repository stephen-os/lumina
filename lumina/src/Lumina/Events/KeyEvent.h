#pragma once

#include "Event.h"

#include <Lumina/Core/KeyCodes.h>
#include <Lumina/Core/Input.h>

#include <sstream>

namespace Lumina
{
    class KeyEvent : public Event
    {
    public:
        KeyCode GetKeyCode() const { return m_KeyCode; }
        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        KeyEvent(KeyCode keycode) : m_KeyCode(keycode) {}
        KeyCode m_KeyCode;
    };

    class KeyPressedEvent : public KeyEvent
    {
    public:
        KeyPressedEvent(KeyCode keycode, bool repeat = false) : KeyEvent(keycode), m_Repeat(repeat) {}
        bool IsRepeat() const { return m_Repeat; }
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << Input::KeyCodeToString(m_KeyCode) << " (repeat = " << (m_Repeat ? "true" : "false") << ")";
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool m_Repeat;
    };

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << Input::KeyCodeToString(m_KeyCode);
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public KeyEvent
    {
    public:
        KeyTypedEvent(KeyCode keycode) : KeyEvent(keycode) {}
        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << Input::KeyCodeToString(m_KeyCode);
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyTyped)
    };
}