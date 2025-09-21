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

namespace Lumina
{
    namespace Assert
    {
        inline void LogFailure(const char* condition, const char* file, int line)
        {
            LUMINA_LOG_CRITICAL("[ASSERT FAILED] Condition: {} | File: {} | Line: {}",
                condition, file, line);
        }

        template<typename... Args>
        inline void LogFailure(const char* condition, const char* file, int line, const std::string& message, Args&&... args)
        {
            std::string formatted_message = Format(message, std::forward<Args>(args)...);
            LUMINA_LOG_CRITICAL("[ASSERT FAILED] Condition: {} | Message: {} | File: {} | Line: {}", condition, formatted_message, file, line);
        }

        inline void LogFailure(const char* condition, const char* file, int line, const std::string& message)
        {
            LUMINA_LOG_CRITICAL("[ASSERT FAILED] Condition: {} | Message: {} | File: {} | Line: {}", condition, message, file, line);
        }
    }
}