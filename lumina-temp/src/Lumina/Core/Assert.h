#pragma once

#include "Log.h"
#include <cstdlib>

#ifdef LUMINA_DEBUG

#define LUMINA_ASSERT(condition, fmt, ...) \
    do { \
        if (!(condition)) { \
            ::Lumina::Assert::LogFailure(#condition, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
            std::abort(); \
        } \
    } while (0)

#else

// No-op in release builds
#define LUMINA_ASSERT(condition, fmt, ...) do { (void)sizeof(condition); } while (0)

#endif

namespace Lumina::Assert
{
    template<typename... Args>
    inline void LogFailure(const char* condition, const char* file, int line, const char* fmt, Args&&... args)
    {
        LUMINA_LOG_ERROR("[ASSERT FAILED] Condition: {}\nMessage: {}\nFile: {}\nLine: {}",
            condition,
            fmt::format(fmt, std::forward<Args>(args)...),
            file,
            line
        );
    }
}
