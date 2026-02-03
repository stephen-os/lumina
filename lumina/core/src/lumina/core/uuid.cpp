#include "uuid.h"

#include <chrono>
#include <random>

namespace lumina::core
{
    std::mutex uuid::s_mutex;

    uint64_t uuid::generate()
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::random_device rd;
        std::mt19937_64 gen(rd());
        uint64_t random_value = gen();
        return now ^ random_value;
    }
}
