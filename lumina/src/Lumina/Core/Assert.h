#pragma once

#include "Log.h"

#include <cstdlib>

#ifdef LUMINA_DEBUG
#define LUMINA_ASSERT(condition, ...) \
        do { \
            if (!(condition)) { \
                ::Lumina::Core::Assert::LogFailure(#condition, __FILE__, __LINE__, ##__VA_ARGS__); \
                std::abort(); \
            } \
        } while (0)
#else
    #define LUMINA_ASSERT(condition, ...) do { (void)sizeof(condition); } while (0)
#endif

namespace Lumina::Core
{
    namespace Assert
    {
        inline void LogFailure(const char* condition, const char* file, int line)
        {
            LUMINA_LOG_CRITICAL("[ASSERT FAILED] Condition: {} | File: {} | Line: {}", condition, file, line);
        }

        template<typename... Args>
        inline void LogFailure(const char* condition, const char* file, int line, const std::string& format, Args&&... args)
        {
            std::string message = fmt::format(fmt::runtime(format), std::forward<Args>(args)...);
            LUMINA_LOG_CRITICAL("[ASSERT FAILED] Condition: {} | Message: {} | File: {} | Line: {}", condition, message, file, line);
        }
    }
}