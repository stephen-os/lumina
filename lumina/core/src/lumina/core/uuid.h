#pragma once

#include <cstdint>
#include <mutex>

namespace lumina::core
{
    class uuid
    {
    public:
        static uint64_t generate();

    private:
        static std::mutex s_mutex;
    };
}
