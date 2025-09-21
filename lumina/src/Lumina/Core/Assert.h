#pragma once
#include "Log.h"
#include <cstdlib>

#ifdef LUMINA_DEBUG
#define LUMINA_ASSERT(condition, ...) \
        do { \
            if (!(condition)) { \
                ::Lumina::Assert::LogFailure(#condition, __FILE__, __LINE__, ##__VA_ARGS__); \
                std::abort(); \
            } \
        } while (0)
#else
    #define LUMINA_ASSERT(condition, ...) do { (void)sizeof(condition); } while (0)
#endif

namespace Lumina::Assert
{
    // Simple version for condition only (no message)
    inline void LogFailure(const char* condition, const char* file, int line)
    {
        LUMINA_LOG_CRITICAL("[ASSERT FAILED] Condition: {} | File: {} | Line: {}",
            condition, file, line);
    }

    // Template version for condition with formatted message
    template<typename... Args>
    inline void LogFailure(const char* condition, const char* file, int line,
        const std::string& format, Args&&... args)
    {
        // Use your ColorFormat to create the message with yellow variables
        std::string formatted_message = ColorFormat(format, std::forward<Args>(args)...);

        LUMINA_LOG_CRITICAL("[ASSERT FAILED] Condition: {} | Message: {} | File: {} | Line: {}",
            condition, formatted_message, file, line);
    }

    // Overload for simple string messages (no formatting)
    inline void LogFailure(const char* condition, const char* file, int line,
        const std::string& message)
    {
        LUMINA_LOG_CRITICAL("[ASSERT FAILED] Condition: {} | Message: {} | File: {} | Line: {}",
            condition, message, file, line);
    }
}