#pragma once

#include "Event.h"

#include "Lumina/Core/KeyCodes.h"
#include "Lumina/Core/Input.h"

#include <sstream>

namespace Lumina
{
    class GlobalKeyEvent : public Event
    {
    public:
        KeyCode GetKeyCode() const { return m_KeyCode; }
        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

    protected:
        GlobalKeyEvent(KeyCode keycode) : m_KeyCode(keycode) {}
        KeyCode m_KeyCode;
    };

    class GlobalKeyPressedEvent : public GlobalKeyEvent
    {
    public:
        GlobalKeyPressedEvent(KeyCode keycode) : GlobalKeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "GlobalKeyPressedEvent: " << Input::KeyCodeToString(m_KeyCode);
            return ss.str();
        }

        EVENT_CLASS_TYPE(GlobalKeyPressed)
    };

    class GlobalKeyReleasedEvent : public GlobalKeyEvent
    {
    public:
        GlobalKeyReleasedEvent(KeyCode keycode) : GlobalKeyEvent(keycode) {}

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "GlobalKeyReleasedEvent: " << Input::KeyCodeToString(m_KeyCode);
            return ss.str();
        }

        EVENT_CLASS_TYPE(GlobalKeyReleased)
    };
}