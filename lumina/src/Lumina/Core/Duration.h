#pragma once

#include <chrono>

namespace Lumina::Core
{
    class Duration
    {
    public:
        constexpr Duration() : m_Milliseconds(0) {}

        constexpr float Milliseconds() const { return m_Milliseconds; }
        constexpr float Seconds() const { return m_Milliseconds / 1000.0f; }

        std::chrono::milliseconds ToChronoMilliseconds() const
        {
            return std::chrono::milliseconds(static_cast<long long>(m_Milliseconds));
        }

        std::chrono::duration<float> ToChronoSeconds() const
        {
            return std::chrono::duration<float>(Seconds());
        }

        static constexpr Duration Milliseconds(float ms) { return Duration(ms); }
        static constexpr Duration Seconds(float s) { return Duration(s * 1000.0f); }
        static constexpr Duration Minutes(float m) { return Duration(m * 60000.0f); }
        static constexpr Duration Hours(float h) { return Duration(h * 3600000.0f); }

        constexpr bool operator==(const Duration& other) const { return m_Milliseconds == other.m_Milliseconds; }
        constexpr bool operator!=(const Duration& other) const { return m_Milliseconds != other.m_Milliseconds; }
        constexpr bool operator<(const Duration& other) const { return m_Milliseconds < other.m_Milliseconds; }
        constexpr bool operator>(const Duration& other) const { return m_Milliseconds > other.m_Milliseconds; }
        constexpr bool operator<=(const Duration& other) const { return m_Milliseconds <= other.m_Milliseconds; }
        constexpr bool operator>=(const Duration& other) const { return m_Milliseconds >= other.m_Milliseconds; }

        constexpr Duration operator+(const Duration& other) const { return Duration(m_Milliseconds + other.m_Milliseconds); }
        constexpr Duration operator-(const Duration& other) const { return Duration(m_Milliseconds - other.m_Milliseconds); }
        constexpr Duration operator*(float scalar) const { return Duration(m_Milliseconds * scalar); }
        constexpr Duration operator/(float scalar) const { return Duration(m_Milliseconds / scalar); }

        constexpr Duration& operator+=(const Duration& other) { m_Milliseconds += other.m_Milliseconds; return *this; }
        constexpr Duration& operator-=(const Duration& other) { m_Milliseconds -= other.m_Milliseconds; return *this; }
        constexpr Duration& operator*=(float scalar) { m_Milliseconds *= scalar; return *this; }
        constexpr Duration& operator/=(float scalar) { m_Milliseconds /= scalar; return *this; }

    private:
        constexpr explicit Duration(float milliseconds) : m_Milliseconds(milliseconds) {}

        float m_Milliseconds;
    };

    inline namespace Literals
    {
        constexpr Duration operator""_ms(unsigned long long ms)
        {
            return Duration::Milliseconds(static_cast<float>(ms));
        }

        constexpr Duration operator""_ms(long double ms)
        {
            return Duration::Milliseconds(static_cast<float>(ms));
        }

        constexpr Duration operator""_s(unsigned long long s)
        {
            return Duration::Seconds(static_cast<float>(s));
        }

        constexpr Duration operator""_s(long double s)
        {
            return Duration::Seconds(static_cast<float>(s));
        }

        constexpr Duration operator""_min(unsigned long long m)
        {
            return Duration::Minutes(static_cast<float>(m));
        }

        constexpr Duration operator""_min(long double m)
        {
            return Duration::Minutes(static_cast<float>(m));
        }

        constexpr Duration operator""_h(unsigned long long h)
        {
            return Duration::Hours(static_cast<float>(h));
        }

        constexpr Duration operator""_h(long double h)
        {
            return Duration::Hours(static_cast<float>(h));
        }
    }
}