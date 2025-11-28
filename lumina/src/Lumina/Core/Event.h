#pragma once

#include <string>
#include <string_view>
#include <format>
#include <functional>

#include "Input.h"

namespace Lumina::Core
{
    constexpr uint32_t BIT(uint32_t x) { return 1u << x; }

    enum class EventType
    {
        None = 0,

        WindowClose,
        WindowResize,
        WindowFocus,
        WindowLostFocus,
        WindowMoved,

        WindowKeyPressed,
        WindowKeyReleased,
        WindowKeyTyped,

        WindowMouseButtonPressed,
        WindowMouseButtonReleased,
        WindowMouseMoved,
        WindowMouseScrolled,

        GlobalKeyPressed,
        GlobalKeyReleased,

        GlobalMouseButtonPressed,
        GlobalMouseButtonReleased,
        GlobalMouseMoved,
        GlobalMouseScrolled,
    };

    constexpr std::string_view EventTypeToString(EventType type)
    {
        switch (type)
        {
        case EventType::None: return "None";
        case EventType::WindowClose: return "WindowClose";
        case EventType::WindowResize: return "WindowResize";
        case EventType::WindowFocus: return "WindowFocus";
        case EventType::WindowLostFocus: return "WindowLostFocus";
        case EventType::WindowMoved: return "WindowMoved";
        case EventType::WindowKeyPressed: return "WindowKeyPressed";
        case EventType::WindowKeyReleased: return "WindowKeyReleased";
        case EventType::WindowKeyTyped: return "WindowKeyTyped";
        case EventType::WindowMouseButtonPressed: return "WindowMouseButtonPressed";
        case EventType::WindowMouseButtonReleased: return "WindowMouseButtonReleased";
        case EventType::WindowMouseMoved: return "WindowMouseMoved";
        case EventType::WindowMouseScrolled: return "WindowMouseScrolled";
        case EventType::GlobalKeyPressed: return "GlobalKeyPressed";
        case EventType::GlobalKeyReleased: return "GlobalKeyReleased";
        case EventType::GlobalMouseButtonPressed: return "GlobalMouseButtonPressed";
        case EventType::GlobalMouseButtonReleased: return "GlobalMouseButtonReleased";
        case EventType::GlobalMouseMoved: return "GlobalMouseMoved";
        case EventType::GlobalMouseScrolled: return "GlobalMouseScrolled";
        default: return "Unknown";
        }
	}

    enum class EventCategory : uint32_t
    {
        None = 0,
        Application = BIT(0),
        Input = BIT(1),
        Keyboard = BIT(2),
        Mouse = BIT(3),
        MouseButton = BIT(4)
    };

    constexpr EventCategory operator|(EventCategory a, EventCategory b)
    {
        return static_cast<EventCategory>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

    constexpr EventCategory operator&(EventCategory a, EventCategory b)
    {
        return static_cast<EventCategory>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}

    constexpr bool operator!=(EventCategory a, EventCategory b)
    {
        return static_cast<uint32_t>(a) != static_cast<uint32_t>(b);
    }

	// Event Base Class
    class Event
    {
    public:
        Event() = default;
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;
        Event(Event&&) = default;
        Event& operator=(Event&&) = default;
        virtual ~Event() = default;

        virtual EventType GetEventType() const = 0;
		virtual EventCategory GetEventCategories() const = 0;
        virtual std::string_view GetName() const = 0;
        virtual std::string ToString() const { return std::string(GetName()); }

        bool IsInCategory(EventCategory category) const { return (GetEventCategories() & category) != EventCategory::None; }

        bool IsHandled() const { return m_Handled; }
		void MarkHandled() { m_Handled = true; }

    private:
        bool m_Handled = false;
    };

    template<typename Derived, EventType Type, EventCategory Categories>
    class EventBase : public Event
    {
    public:
        static constexpr EventType GetStaticType() { return Type; }
		static constexpr EventCategory GetStaticCategories() { return Categories; }
		
        EventType GetEventType() const override { return Type; }
		EventCategory GetEventCategories() const override { return Categories; }
        std::string_view GetName() const override { return EventTypeToString(Type); }
    };

    template<typename Derived, EventType Type>
    class KeyEventBase : public EventBase<Derived, Type, EventCategory::Input | EventCategory::Keyboard>
    {
    public:
        KeyEventBase(Input::KeyCode keyCode) : m_KeyCode(keyCode) {}

        Input::KeyCode GetKeyCode() const { return m_KeyCode; }

        std::string ToString() const override
        {
            return std::string(this->GetName()) + ": " + std::string(Input::KeyCodeToString(m_KeyCode));
        }

    protected:
        Input::KeyCode m_KeyCode;
    };

    template<typename Derived, EventType Type>
    class MouseButtonEventBase : public EventBase<Derived, Type, EventCategory::Input | EventCategory::Mouse | EventCategory::MouseButton>
    {
    public:
        MouseButtonEventBase(Input::MouseCode button) : m_Button(button) {}

        Input::MouseCode GetButton() const { return m_Button; }

        std::string ToString() const override
        {
            return std::string(this->GetName()) + ": " + std::string(Input::MouseCodeToString(m_Button));
        }

    protected:
        Input::MouseCode m_Button;
    };

    template<typename Derived, EventType Type>
    class MouseMoveEventBase : public EventBase<Derived, Type, EventCategory::Input | EventCategory::Mouse>
    {
    public:
        MouseMoveEventBase(float x, float y) : m_X(x), m_Y(y) {}

        float GetX() const { return m_X; }
        float GetY() const { return m_Y; }

        std::string ToString() const override
        {
            return std::format("{}: {}, {}", this->GetName(), m_X, m_Y);
        }

    protected:
        float m_X, m_Y;
    };

    // Events 
    class WindowCloseEvent : public EventBase<WindowCloseEvent, EventType::WindowClose, EventCategory::Application>
    {
    public:
        WindowCloseEvent() = default;
    };

    class WindowResizeEvent : public EventBase<WindowResizeEvent, EventType::WindowResize, EventCategory::Application>
    {
    public:
        WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) { }

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        std::string ToString() const override
        {
            return std::format("{}: {}x{}", GetName(), m_Width, m_Height);
        }

    private:
        uint32_t m_Width, m_Height;
    };

    class WindowFocusEvent : public EventBase<WindowFocusEvent, EventType::WindowFocus, EventCategory::Application>
    {
    public:
        WindowFocusEvent() = default;
    };

    class WindowLostFocusEvent : public EventBase<WindowLostFocusEvent, EventType::WindowLostFocus, EventCategory::Application>
    {
    public:
        WindowLostFocusEvent() = default;
    };

    class WindowMovedEvent : public EventBase<WindowMovedEvent, EventType::WindowMoved, EventCategory::Application>
    {
    public:
        WindowMovedEvent(int32_t x, int32_t y) : m_X(x), m_Y(y) { }

        int32_t GetX() const { return m_X; }
        int32_t GetY() const { return m_Y; }

        std::string ToString() const override
        {
            return std::format("{}: {}, {}", GetName(), m_X, m_Y);
        }

    private:
        int32_t m_X, m_Y;
    };

    class WindowKeyPressedEvent : public KeyEventBase<WindowKeyPressedEvent,
        EventType::WindowKeyPressed>
    {
    public:
        WindowKeyPressedEvent(Input::KeyCode keyCode, bool isRepeat = false) : KeyEventBase(keyCode), m_IsRepeat(isRepeat) { }

        bool IsRepeat() const { return m_IsRepeat; }

        std::string ToString() const override
        {
            return std::string(this->GetName()) + ": " + std::string(Input::KeyCodeToString(m_KeyCode)) + (m_IsRepeat ? " (repeat)" : "");
        }

    private:
        bool m_IsRepeat;
    };

    class WindowKeyReleasedEvent : public KeyEventBase<WindowKeyReleasedEvent, EventType::WindowKeyReleased>
    {
    public:
        WindowKeyReleasedEvent(Input::KeyCode keyCode) : KeyEventBase(keyCode) {}
    };

    class WindowKeyTypedEvent : public EventBase<WindowKeyTypedEvent, EventType::WindowKeyTyped, EventCategory::Input | EventCategory::Keyboard>
    {
    public:
        WindowKeyTypedEvent(uint32_t character) : m_Character(character) {}

        uint32_t GetCharacter() const { return m_Character; }

        std::string ToString() const override
        {
            return std::string(this->GetName()) + ": " + std::to_string(m_Character);
        }

    private:
        uint32_t m_Character;
    };

    class WindowMouseButtonPressedEvent : public MouseButtonEventBase<WindowMouseButtonPressedEvent, EventType::WindowMouseButtonPressed>
    {
    public:
        WindowMouseButtonPressedEvent(Input::MouseCode button) : MouseButtonEventBase(button) {}
    };

    class WindowMouseButtonReleasedEvent : public MouseButtonEventBase<WindowMouseButtonReleasedEvent, EventType::WindowMouseButtonReleased>
    {
    public:
        WindowMouseButtonReleasedEvent(Input::MouseCode button) : MouseButtonEventBase(button) {}
    };

    class WindowMouseMovedEvent : public MouseMoveEventBase<WindowMouseMovedEvent, EventType::WindowMouseMoved>
    {
    public:
        WindowMouseMovedEvent(float x, float y) : MouseMoveEventBase(x, y) {}
    };

    class WindowMouseScrolledEvent : public EventBase<WindowMouseScrolledEvent, EventType::WindowMouseScrolled, EventCategory::Input | EventCategory::Mouse>
    {
    public:
        WindowMouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) { }

        float GetXOffset() const { return m_XOffset; }
        float GetYOffset() const { return m_YOffset; }

        std::string ToString() const override
        {
            return std::format("{}: {}, {}", GetName(), m_XOffset, m_YOffset);
        }

    private:
        float m_XOffset, m_YOffset;
    };

    class GlobalKeyPressedEvent : public KeyEventBase<GlobalKeyPressedEvent, EventType::GlobalKeyPressed>
    {
    public:
        GlobalKeyPressedEvent(Input::KeyCode keyCode) : KeyEventBase(keyCode) {}
    };

    class GlobalKeyReleasedEvent : public KeyEventBase<GlobalKeyReleasedEvent, EventType::GlobalKeyReleased>
    {
    public:
        GlobalKeyReleasedEvent(Input::KeyCode keyCode) : KeyEventBase(keyCode) {}
    };

    class GlobalMouseButtonPressedEvent : public MouseButtonEventBase<GlobalMouseButtonPressedEvent, EventType::GlobalMouseButtonPressed>
    {
    public:
        GlobalMouseButtonPressedEvent(Input::MouseCode button) : MouseButtonEventBase(button) {}
    };

    class GlobalMouseButtonReleasedEvent : public MouseButtonEventBase<GlobalMouseButtonReleasedEvent, EventType::GlobalMouseButtonReleased>
    {
    public:
        GlobalMouseButtonReleasedEvent(Input::MouseCode button) : MouseButtonEventBase(button) {}
    };

    class GlobalMouseMovedEvent : public MouseMoveEventBase<GlobalMouseMovedEvent, EventType::GlobalMouseMoved>
    {
    public:
        GlobalMouseMovedEvent(float x, float y) : MouseMoveEventBase(x, y) {}
    };

    class GlobalMouseScrolledEvent : public EventBase<GlobalMouseScrolledEvent, EventType::GlobalMouseScrolled, EventCategory::Input | EventCategory::Mouse>
    {
    public:
        GlobalMouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) { }

        float GetXOffset() const { return m_XOffset; }
        float GetYOffset() const { return m_YOffset; }

        std::string ToString() const override
        {
            return std::format("{}: {}, {}", GetName(), m_XOffset, m_YOffset);
        }

    private:
        float m_XOffset, m_YOffset;
    };

	// Event Dispatcher
    template<typename T>
    concept IsEvent = std::is_base_of_v<Event, T>;

    class EventDispatcher
    {
    public:
        explicit EventDispatcher(Event& event) : m_Event(event) {}

        template<IsEvent T, typename F>
        requires std::invocable<F, T&>
        bool Dispatch(F&& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                bool handled = std::invoke(std::forward<F>(func), static_cast<T&>(m_Event));
                if (handled)
                    m_Event.MarkHandled();
                return true;
            }
            return false;
        }

    private:
        Event& m_Event;
    };
}