#pragma once

#include <chrono>
#include <random>
#include <mutex>

namespace Lumina
{
    class UUID
    {
    public:
        static uint64_t Generate();
    private:
        static std::mutex m_Mutex;
    };
}