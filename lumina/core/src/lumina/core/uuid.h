#pragma once

#include <cstdint>

namespace lumina::core
{
    class uuid
    {
    public:
        [[nodiscard]] static uint64_t generate();
    };
}
