#pragma once

#include "input.h"

#include <string>
#include <string_view>
#include <format>
#include <functional>

namespace lumina::core
{
    constexpr uint32_t bit(uint32_t x) { return 1u << x; }

    enum class event_type
    {
        none = 0,

        window_close,
        window_resize,
        window_focus,
        window_lost_focus,
        window_moved,

        key_pressed,
        key_released,
        key_typed,

        mouse_button_pressed,
        mouse_button_released,
        mouse_moved,
        mouse_scrolled,
    };

    constexpr std::string_view event_type_to_string(event_type type)
    {
        switch (type)
        {
        case event_type::none: return "none";
        case event_type::window_close: return "window_close";
        case event_type::window_resize: return "window_resize";
        case event_type::window_focus: return "window_focus";
        case event_type::window_lost_focus: return "window_lost_focus";
        case event_type::window_moved: return "window_moved";
        case event_type::key_pressed: return "key_pressed";
        case event_type::key_released: return "key_released";
        case event_type::key_typed: return "key_typed";
        case event_type::mouse_button_pressed: return "mouse_button_pressed";
        case event_type::mouse_button_released: return "mouse_button_released";
        case event_type::mouse_moved: return "mouse_moved";
        case event_type::mouse_scrolled: return "mouse_scrolled";
        default: return "unknown";
        }
    }

    enum class event_category : uint32_t
    {
        none = 0,
        application = bit(0),
        input = bit(1),
        keyboard = bit(2),
        mouse = bit(3),
        mouse_button = bit(4)
    };

    constexpr event_category operator|(event_category a, event_category b)
    {
        return static_cast<event_category>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    constexpr event_category operator&(event_category a, event_category b)
    {
        return static_cast<event_category>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    constexpr bool operator!=(event_category a, event_category b)
    {
        return static_cast<uint32_t>(a) != static_cast<uint32_t>(b);
    }

    class event
    {
    public:
        event() = default;
        event(const event&) = delete;
        event& operator=(const event&) = delete;
        event(event&&) = default;
        event& operator=(event&&) = default;
        virtual ~event() = default;

        virtual event_type get_type() const = 0;
        virtual event_category get_categories() const = 0;
        virtual std::string_view get_name() const = 0;
        virtual std::string to_string() const { return std::string(get_name()); }

        bool is_in_category(event_category category) const
        {
            return (get_categories() & category) != event_category::none;
        }

        bool is_handled() const { return m_handled; }
        void mark_handled() { m_handled = true; }

    private:
        bool m_handled = false;
    };

    template<typename Derived, event_type Type, event_category Categories>
    class event_base : public event
    {
    public:
        static constexpr event_type static_type() { return Type; }
        static constexpr event_category static_categories() { return Categories; }

        event_type get_type() const override { return Type; }
        event_category get_categories() const override { return Categories; }
        std::string_view get_name() const override { return event_type_to_string(Type); }
    };

    template<typename Derived, event_type Type>
    class key_event_base : public event_base<Derived, Type, event_category::input | event_category::keyboard>
    {
    public:
        key_event_base(input::key_code key) : m_key(key) {}

        input::key_code get_key() const { return m_key; }

        std::string to_string() const override
        {
            return std::string(this->get_name()) + ": " + std::string(input::key_code_to_string(m_key));
        }

    protected:
        input::key_code m_key;
    };

    template<typename Derived, event_type Type>
    class mouse_button_event_base : public event_base<Derived, Type, event_category::input | event_category::mouse | event_category::mouse_button>
    {
    public:
        mouse_button_event_base(input::mouse_code button) : m_button(button) {}

        input::mouse_code get_button() const { return m_button; }

        std::string to_string() const override
        {
            return std::string(this->get_name()) + ": " + std::string(input::mouse_code_to_string(m_button));
        }

    protected:
        input::mouse_code m_button;
    };

    // Window events
    class window_close_event final : public event_base<window_close_event, event_type::window_close, event_category::application>
    {
    public:
        window_close_event() = default;
    };

    class window_resize_event final : public event_base<window_resize_event, event_type::window_resize, event_category::application>
    {
    public:
        window_resize_event(uint32_t width, uint32_t height) : m_width(width), m_height(height) {}

        uint32_t get_width() const { return m_width; }
        uint32_t get_height() const { return m_height; }

        std::string to_string() const override
        {
            return std::format("{}: {}x{}", get_name(), m_width, m_height);
        }

    private:
        uint32_t m_width, m_height;
    };

    class window_focus_event final : public event_base<window_focus_event, event_type::window_focus, event_category::application>
    {
    public:
        window_focus_event() = default;
    };

    class window_lost_focus_event final : public event_base<window_lost_focus_event, event_type::window_lost_focus, event_category::application>
    {
    public:
        window_lost_focus_event() = default;
    };

    class window_moved_event final : public event_base<window_moved_event, event_type::window_moved, event_category::application>
    {
    public:
        window_moved_event(int32_t x, int32_t y) : m_x(x), m_y(y) {}

        int32_t get_x() const { return m_x; }
        int32_t get_y() const { return m_y; }

        std::string to_string() const override
        {
            return std::format("{}: {}, {}", get_name(), m_x, m_y);
        }

    private:
        int32_t m_x, m_y;
    };

    // Keyboard events
    class key_pressed_event final : public key_event_base<key_pressed_event, event_type::key_pressed>
    {
    public:
        key_pressed_event(input::key_code key, bool is_repeat = false)
            : key_event_base(key), m_is_repeat(is_repeat) {}

        bool is_repeat() const { return m_is_repeat; }

        std::string to_string() const override
        {
            return std::string(this->get_name()) + ": " +
                   std::string(input::key_code_to_string(m_key)) +
                   (m_is_repeat ? " (repeat)" : "");
        }

    private:
        bool m_is_repeat;
    };

    class key_released_event final : public key_event_base<key_released_event, event_type::key_released>
    {
    public:
        key_released_event(input::key_code key) : key_event_base(key) {}
    };

    class key_typed_event final : public event_base<key_typed_event, event_type::key_typed, event_category::input | event_category::keyboard>
    {
    public:
        key_typed_event(uint32_t character) : m_character(character) {}

        uint32_t get_character() const { return m_character; }

        std::string to_string() const override
        {
            return std::string(this->get_name()) + ": " + std::to_string(m_character);
        }

    private:
        uint32_t m_character;
    };

    // Mouse events
    class mouse_button_pressed_event final : public mouse_button_event_base<mouse_button_pressed_event, event_type::mouse_button_pressed>
    {
    public:
        mouse_button_pressed_event(input::mouse_code button) : mouse_button_event_base(button) {}
    };

    class mouse_button_released_event final : public mouse_button_event_base<mouse_button_released_event, event_type::mouse_button_released>
    {
    public:
        mouse_button_released_event(input::mouse_code button) : mouse_button_event_base(button) {}
    };

    class mouse_moved_event final : public event_base<mouse_moved_event, event_type::mouse_moved, event_category::input | event_category::mouse>
    {
    public:
        mouse_moved_event(float x, float y) : m_x(x), m_y(y) {}

        float get_x() const { return m_x; }
        float get_y() const { return m_y; }

        std::string to_string() const override
        {
            return std::format("{}: {}, {}", get_name(), m_x, m_y);
        }

    private:
        float m_x, m_y;
    };

    class mouse_scrolled_event final : public event_base<mouse_scrolled_event, event_type::mouse_scrolled, event_category::input | event_category::mouse>
    {
    public:
        mouse_scrolled_event(float x_offset, float y_offset)
            : m_x_offset(x_offset), m_y_offset(y_offset) {}

        float get_x_offset() const { return m_x_offset; }
        float get_y_offset() const { return m_y_offset; }

        std::string to_string() const override
        {
            return std::format("{}: {}, {}", get_name(), m_x_offset, m_y_offset);
        }

    private:
        float m_x_offset, m_y_offset;
    };

    // Event dispatcher
    template<typename T>
    concept is_event = std::is_base_of_v<event, T>;

    class event_dispatcher
    {
    public:
        explicit event_dispatcher(event& e) : m_event(e) {}

        template<is_event T, typename F>
        requires std::invocable<F, T&>
        bool dispatch(F&& func)
        {
            if (m_event.get_type() == T::static_type())
            {
                bool handled = std::invoke(std::forward<F>(func), static_cast<T&>(m_event));
                if (handled)
                    m_event.mark_handled();
                return true;
            }
            return false;
        }

    private:
        event& m_event;
    };
}
