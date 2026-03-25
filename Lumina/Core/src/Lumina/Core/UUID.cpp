#include "UUID.h"

#include <chrono>
#include <random>

namespace Lumina
{
	uint64_t UUID::Generate()
	{
		thread_local std::mt19937_64 generator = []() {
			std::random_device rd;
			return std::mt19937_64(rd());
		}();

		auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		uint64_t randomValue = generator();
		return now ^ randomValue;
	}
}
