#pragma once

#include <chrono>

namespace lumina::core
{
    class duration
    {
    public:
        constexpr duration() : m_milliseconds(0) {}

        constexpr float milliseconds() const { return m_milliseconds; }
        constexpr float seconds() const { return m_milliseconds / 1000.0f; }

        std::chrono::milliseconds to_chrono_milliseconds() const
        {
            return std::chrono::milliseconds(static_cast<long long>(m_milliseconds));
        }

        std::chrono::duration<float> to_chrono_seconds() const
        {
            return std::chrono::duration<float>(seconds());
        }

        static constexpr duration from_milliseconds(float ms) { return duration(ms); }
        static constexpr duration from_seconds(float s) { return duration(s * 1000.0f); }
        static constexpr duration from_minutes(float m) { return duration(m * 60000.0f); }
        static constexpr duration from_hours(float h) { return duration(h * 3600000.0f); }

        constexpr bool operator==(const duration& other) const { return m_milliseconds == other.m_milliseconds; }
        constexpr bool operator!=(const duration& other) const { return m_milliseconds != other.m_milliseconds; }
        constexpr bool operator<(const duration& other) const { return m_milliseconds < other.m_milliseconds; }
        constexpr bool operator>(const duration& other) const { return m_milliseconds > other.m_milliseconds; }
        constexpr bool operator<=(const duration& other) const { return m_milliseconds <= other.m_milliseconds; }
        constexpr bool operator>=(const duration& other) const { return m_milliseconds >= other.m_milliseconds; }

        constexpr duration operator+(const duration& other) const { return duration(m_milliseconds + other.m_milliseconds); }
        constexpr duration operator-(const duration& other) const { return duration(m_milliseconds - other.m_milliseconds); }
        constexpr duration operator*(float scalar) const { return duration(m_milliseconds * scalar); }
        constexpr duration operator/(float scalar) const { return duration(m_milliseconds / scalar); }

        constexpr duration& operator+=(const duration& other) { m_milliseconds += other.m_milliseconds; return *this; }
        constexpr duration& operator-=(const duration& other) { m_milliseconds -= other.m_milliseconds; return *this; }
        constexpr duration& operator*=(float scalar) { m_milliseconds *= scalar; return *this; }
        constexpr duration& operator/=(float scalar) { m_milliseconds /= scalar; return *this; }

    private:
        constexpr explicit duration(float milliseconds) : m_milliseconds(milliseconds) {}

        float m_milliseconds;
    };

    inline namespace literals
    {
        constexpr duration operator""_ms(unsigned long long ms)
        {
            return duration::from_milliseconds(static_cast<float>(ms));
        }

        constexpr duration operator""_ms(long double ms)
        {
            return duration::from_milliseconds(static_cast<float>(ms));
        }

        constexpr duration operator""_s(unsigned long long s)
        {
            return duration::from_seconds(static_cast<float>(s));
        }

        constexpr duration operator""_s(long double s)
        {
            return duration::from_seconds(static_cast<float>(s));
        }

        constexpr duration operator""_min(unsigned long long m)
        {
            return duration::from_minutes(static_cast<float>(m));
        }

        constexpr duration operator""_min(long double m)
        {
            return duration::from_minutes(static_cast<float>(m));
        }

        constexpr duration operator""_h(unsigned long long h)
        {
            return duration::from_hours(static_cast<float>(h));
        }

        constexpr duration operator""_h(long double h)
        {
            return duration::from_hours(static_cast<float>(h));
        }
    }
}
