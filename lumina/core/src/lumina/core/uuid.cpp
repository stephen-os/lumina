#include "uuid.h"

#include <chrono>
#include <random>

namespace lumina::core
{
    uint64_t uuid::generate()
    {
        thread_local std::mt19937_64 generator = []() {
            std::random_device rd;
            return std::mt19937_64(rd());
        }();

        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        uint64_t random_value = generator();
        return now ^ random_value;
    }
}
