#pragma once

#include "Log.h"

#include <chrono>
#include <string>

namespace Lumina
{
	class Timer
	{
	public:
		Timer() { Reset(); }

		void Reset()
		{
			m_Start = std::chrono::high_resolution_clock::now();
		}

		[[nodiscard]] float Elapsed() const
		{
			auto now = std::chrono::high_resolution_clock::now();
			return std::chrono::duration<float>(now - m_Start).count();
		}

		[[nodiscard]] float ElapsedMillis() const
		{
			return Elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
	};

	class ScopedTimer
	{
	public:
		explicit ScopedTimer(const std::string& name)
			: m_Name(name) {}

		~ScopedTimer()
		{
			float ms = m_Timer.ElapsedMillis();
			LUMINA_LOG_TRACE("[TIMER] {} - {:.3f}ms", m_Name, ms);
		}

		ScopedTimer(const ScopedTimer&) = delete;
		ScopedTimer& operator=(const ScopedTimer&) = delete;

	private:
		std::string m_Name;
		Timer m_Timer;
	};
}
