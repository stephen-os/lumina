#pragma once

#include <chrono>
#include <random>

#include <mutex>

namespace Lumina
{
	class UUID
	{
	public:
		static uint64_t Generate()
		{
			std::lock_guard<std::mutex> lock(m_Mutex);

			auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			std::random_device rd;
			std::mt19937_64 gen(rd());
			uint64_t randomValue = gen();
			return now ^ randomValue;
		}
	private:
		static std::mutex m_Mutex; 
	};

	std::mutex UUID::m_Mutex; 
}