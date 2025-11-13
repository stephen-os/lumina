#include "UUID.h"

namespace Lumina
{
    std::mutex UUID::m_Mutex;

    uint64_t UUID::Generate()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::random_device rd;
        std::mt19937_64 gen(rd());
        uint64_t randomValue = gen();
        return now ^ randomValue;
    }
}