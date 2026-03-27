#pragma once

#include "Input.h"

#include <string>
#include <string_view>
#include <format>
#include <functional>

namespace Lumina
{
	constexpr uint32_t Bit(uint32_t x) { return 1u << x; }

	enum class EventType
	{
		None = 0,

		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,

		KeyPressed,
		KeyReleased,
		KeyTyped,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,
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
		case EventType::KeyPressed: return "KeyPressed";
		case EventType::KeyReleased: return "KeyReleased";
		case EventType::KeyTyped: return "KeyTyped";
		case EventType::MouseButtonPressed: return "MouseButtonPressed";
		case EventType::MouseButtonReleased: return "MouseButtonReleased";
		case EventType::MouseMoved: return "MouseMoved";
		case EventType::MouseScrolled: return "MouseScrolled";
		default: return "Unknown";
		}
	}

	enum class EventCategory : uint32_t
	{
		None = 0,
		Application = Bit(0),
		Input = Bit(1),
		Keyboard = Bit(2),
		Mouse = Bit(3),
		MouseButton = Bit(4)
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

	class Event
	{
	public:
		Event() = default;
		Event(const Event&) = delete;
		Event& operator=(const Event&) = delete;
		Event(Event&&) = default;
		Event& operator=(Event&&) = default;
		virtual ~Event() = default;

		virtual EventType GetType() const = 0;
		virtual EventCategory GetCategories() const = 0;
		virtual std::string_view GetName() const = 0;
		virtual std::string ToString() const { return std::string(GetName()); }

		bool IsInCategory(EventCategory category) const
		{
			return (GetCategories() & category) != EventCategory::None;
		}

		bool IsHandled() const { return m_Handled; }
		void MarkHandled() { m_Handled = true; }

	private:
		bool m_Handled = false;
	};

	template<typename Derived, EventType Type, EventCategory Categories>
	class EventBase : public Event
	{
	public:
		static constexpr EventType StaticType() { return Type; }
		static constexpr EventCategory StaticCategories() { return Categories; }

		EventType GetType() const override { return Type; }
		EventCategory GetCategories() const override { return Categories; }
		std::string_view GetName() const override { return EventTypeToString(Type); }
	};

	template<typename Derived, EventType Type>
	class KeyEventBase : public EventBase<Derived, Type, EventCategory::Input | EventCategory::Keyboard>
	{
	public:
		KeyEventBase(Input::KeyCode key) : m_Key(key) {}

		Input::KeyCode GetKey() const { return m_Key; }

		std::string ToString() const override
		{
			return std::string(this->GetName()) + ": " + std::string(Input::KeyCodeToString(m_Key));
		}

	protected:
		Input::KeyCode m_Key;
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

	// Window events
	class WindowCloseEvent final : public EventBase<WindowCloseEvent, EventType::WindowClose, EventCategory::Application>
	{
	public:
		WindowCloseEvent() = default;
	};

	class WindowResizeEvent final : public EventBase<WindowResizeEvent, EventType::WindowResize, EventCategory::Application>
	{
	public:
		WindowResizeEvent(uint32_t width, uint32_t height) : m_Width(width), m_Height(height) {}

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			return std::format("{}: {}x{}", GetName(), m_Width, m_Height);
		}

	private:
		uint32_t m_Width, m_Height;
	};

	class WindowFocusEvent final : public EventBase<WindowFocusEvent, EventType::WindowFocus, EventCategory::Application>
	{
	public:
		WindowFocusEvent() = default;
	};

	class WindowLostFocusEvent final : public EventBase<WindowLostFocusEvent, EventType::WindowLostFocus, EventCategory::Application>
	{
	public:
		WindowLostFocusEvent() = default;
	};

	class WindowMovedEvent final : public EventBase<WindowMovedEvent, EventType::WindowMoved, EventCategory::Application>
	{
	public:
		WindowMovedEvent(int32_t x, int32_t y) : m_X(x), m_Y(y) {}

		int32_t GetX() const { return m_X; }
		int32_t GetY() const { return m_Y; }

		std::string ToString() const override
		{
			return std::format("{}: {}, {}", GetName(), m_X, m_Y);
		}

	private:
		int32_t m_X, m_Y;
	};

	// Keyboard events
	class KeyPressedEvent final : public KeyEventBase<KeyPressedEvent, EventType::KeyPressed>
	{
	public:
		KeyPressedEvent(Input::KeyCode key, bool isRepeat = false)
			: KeyEventBase(key), m_IsRepeat(isRepeat) {}

		bool IsRepeat() const { return m_IsRepeat; }

		std::string ToString() const override
		{
			return std::string(this->GetName()) + ": " +
			       std::string(Input::KeyCodeToString(m_Key)) +
			       (m_IsRepeat ? " (repeat)" : "");
		}

	private:
		bool m_IsRepeat;
	};

	class KeyReleasedEvent final : public KeyEventBase<KeyReleasedEvent, EventType::KeyReleased>
	{
	public:
		KeyReleasedEvent(Input::KeyCode key) : KeyEventBase(key) {}
	};

	class KeyTypedEvent final : public EventBase<KeyTypedEvent, EventType::KeyTyped, EventCategory::Input | EventCategory::Keyboard>
	{
	public:
		KeyTypedEvent(uint32_t character) : m_Character(character) {}

		uint32_t GetCharacter() const { return m_Character; }

		std::string ToString() const override
		{
			return std::string(this->GetName()) + ": " + std::to_string(m_Character);
		}

	private:
		uint32_t m_Character;
	};

	// Mouse events
	class MouseButtonPressedEvent final : public MouseButtonEventBase<MouseButtonPressedEvent, EventType::MouseButtonPressed>
	{
	public:
		MouseButtonPressedEvent(Input::MouseCode button) : MouseButtonEventBase(button) {}
	};

	class MouseButtonReleasedEvent final : public MouseButtonEventBase<MouseButtonReleasedEvent, EventType::MouseButtonReleased>
	{
	public:
		MouseButtonReleasedEvent(Input::MouseCode button) : MouseButtonEventBase(button) {}
	};

	class MouseMovedEvent final : public EventBase<MouseMovedEvent, EventType::MouseMoved, EventCategory::Input | EventCategory::Mouse>
	{
	public:
		MouseMovedEvent(float x, float y) : m_X(x), m_Y(y) {}

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }

		std::string ToString() const override
		{
			return std::format("{}: {}, {}", GetName(), m_X, m_Y);
		}

	private:
		float m_X, m_Y;
	};

	class MouseScrolledEvent final : public EventBase<MouseScrolledEvent, EventType::MouseScrolled, EventCategory::Input | EventCategory::Mouse>
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			return std::format("{}: {}, {}", GetName(), m_XOffset, m_YOffset);
		}

	private:
		float m_XOffset, m_YOffset;
	};

	// Event dispatcher
	template<typename T>
	concept IsEvent = std::is_base_of_v<Event, T>;

	class EventDispatcher
	{
	public:
		explicit EventDispatcher(Event& e) : m_Event(e) {}

		template<IsEvent T, typename F>
		requires std::invocable<F, T&>
		bool Dispatch(F&& func)
		{
			if (m_Event.GetType() == T::StaticType())
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
