#pragma once

#include <chrono>

namespace Lumina
{
	class Duration
	{
	public:
		constexpr Duration() : m_Milliseconds(0) {}

		[[nodiscard]] constexpr float Milliseconds() const { return m_Milliseconds; }
		[[nodiscard]] constexpr float Seconds() const { return m_Milliseconds / 1000.0f; }

		[[nodiscard]] std::chrono::milliseconds ToChronoMilliseconds() const
		{
			return std::chrono::milliseconds(static_cast<long long>(m_Milliseconds));
		}

		[[nodiscard]] std::chrono::duration<float> ToChronoSeconds() const
		{
			return std::chrono::duration<float>(Seconds());
		}

		[[nodiscard]] static constexpr Duration FromMilliseconds(float ms) { return Duration(ms); }
		[[nodiscard]] static constexpr Duration FromSeconds(float s) { return Duration(s * 1000.0f); }
		[[nodiscard]] static constexpr Duration FromMinutes(float m) { return Duration(m * 60000.0f); }
		[[nodiscard]] static constexpr Duration FromHours(float h) { return Duration(h * 3600000.0f); }

		[[nodiscard]] constexpr bool operator==(const Duration& other) const { return m_Milliseconds == other.m_Milliseconds; }
		[[nodiscard]] constexpr bool operator!=(const Duration& other) const { return m_Milliseconds != other.m_Milliseconds; }
		[[nodiscard]] constexpr bool operator<(const Duration& other) const { return m_Milliseconds < other.m_Milliseconds; }
		[[nodiscard]] constexpr bool operator>(const Duration& other) const { return m_Milliseconds > other.m_Milliseconds; }
		[[nodiscard]] constexpr bool operator<=(const Duration& other) const { return m_Milliseconds <= other.m_Milliseconds; }
		[[nodiscard]] constexpr bool operator>=(const Duration& other) const { return m_Milliseconds >= other.m_Milliseconds; }

		[[nodiscard]] constexpr Duration operator+(const Duration& other) const { return Duration(m_Milliseconds + other.m_Milliseconds); }
		[[nodiscard]] constexpr Duration operator-(const Duration& other) const { return Duration(m_Milliseconds - other.m_Milliseconds); }
		[[nodiscard]] constexpr Duration operator*(float scalar) const { return Duration(m_Milliseconds * scalar); }
		[[nodiscard]] constexpr Duration operator/(float scalar) const { return Duration(m_Milliseconds / scalar); }

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
			return Duration::FromMilliseconds(static_cast<float>(ms));
		}

		constexpr Duration operator""_ms(long double ms)
		{
			return Duration::FromMilliseconds(static_cast<float>(ms));
		}

		constexpr Duration operator""_s(unsigned long long s)
		{
			return Duration::FromSeconds(static_cast<float>(s));
		}

		constexpr Duration operator""_s(long double s)
		{
			return Duration::FromSeconds(static_cast<float>(s));
		}

		constexpr Duration operator""_min(unsigned long long m)
		{
			return Duration::FromMinutes(static_cast<float>(m));
		}

		constexpr Duration operator""_min(long double m)
		{
			return Duration::FromMinutes(static_cast<float>(m));
		}

		constexpr Duration operator""_h(unsigned long long h)
		{
			return Duration::FromHours(static_cast<float>(h));
		}

		constexpr Duration operator""_h(long double h)
		{
			return Duration::FromHours(static_cast<float>(h));
		}
	}
}
