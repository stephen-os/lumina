#pragma once

#include <string>
#include <functional>

#define BIT(x) (1 << x)

namespace Lumina
{
    enum class EventType
    {
        None = 0,

        // Window events
        WindowClose = 1,
        WindowResize = 2,
        WindowFocus = 3,
        WindowLostFocus = 4,
        WindowMoved = 5,

        // Keyboard events
        KeyPressed = 6,
        KeyReleased = 7,
        KeyTyped = 8,

        // Mouse events
        MouseButtonPressed = 9,
        MouseButtonReleased = 10,
        MouseMoved = 11,
        MouseScrolled = 12,

        // User events can start from this range
        UserEventStart = 1000
    };

    enum EventCategory
    {
        None = 0,
        EventCategoryApplication = BIT(0),
        EventCategoryInput = BIT(1),
        EventCategoryKeyboard = BIT(2),
        EventCategoryMouse = BIT(3),
        EventCategoryMouseButton = BIT(4)
    };

    class Event
    {
    public:
        bool Handled = false;

        virtual ~Event() = default;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual int GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventCategory category) const
        {
            return GetCategoryFlags() & category;
        }
    };

    class EventDispatcher
    {
    public:
        EventDispatcher(Event& event) : m_Event(event) { }

        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.Handled |= func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };

    // Helper macro to reduce boilerplate
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
                                   virtual EventType GetEventType() const override { return GetStaticType(); }\
                                   virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }
}