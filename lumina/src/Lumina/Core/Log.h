#pragma once
#include <memory>
#include <string>
#include <chrono>
#include <sstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace Lumina
{
    namespace LogColors
    {
        constexpr const char* RESET = "\033[0m";
        constexpr const char* WHITE = "\033[37m";
        constexpr const char* GRAY = "\033[90m";
        constexpr const char* GREEN = "\033[32m";
        constexpr const char* YELLOW = "\033[33m";
        constexpr const char* RED = "\033[31m";
        constexpr const char* CYAN = "\033[36m";
        constexpr const char* ORANGE = "\033[38;5;208m";
        constexpr const char* BOLD_RED = "\033[1;31m";
    }

    // C++20 compatible colored string formatter
    class Formatter
    {
    public:
        Formatter(const std::string& format_str) : m_format(format_str) {}

        template<typename T>
        Formatter& operator<<(T&& value)
        {
            ReplaceNextPlaceholder(ToString(std::forward<T>(value)));
            return *this;
        }

        std::string Build() const
        {
            return m_result.empty() ? m_format : m_result;
        }

    private:
        std::string m_format;
        std::string m_result;
        size_t m_current_pos = 0;

        template<typename T>
        std::string ToString(T&& value)
        {
            if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
                return value;
            else if constexpr (std::is_arithmetic_v<std::decay_t<T>>)
                return std::to_string(value);
            else
            {
                std::ostringstream oss;
                oss << value;
                return oss.str();
            }
        }

        void ReplaceNextPlaceholder(const std::string& value)
        {
            if (m_result.empty())
                m_result = m_format;

            size_t pos = m_result.find("{}", m_current_pos);
            if (pos != std::string::npos)
            {
                std::string colored_value = std::string(LogColors::YELLOW) + value + LogColors::WHITE;
                m_result.replace(pos, 2, colored_value);
                m_current_pos = pos + colored_value.length();
            }
        }
    };

    // Helper function to create colored formatted strings
    template<typename... Args>
    std::string Format(const std::string& format_str, Args&&... args)
    {
        Formatter formatter(format_str);
        ((formatter << std::forward<Args>(args)), ...);
        return std::string(LogColors::WHITE) + formatter.Build() + LogColors::RESET;
    }

    class LogFormatter : public spdlog::formatter
    {
    public:
        LogFormatter() = default;
        void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override;
        std::unique_ptr<spdlog::formatter> clone() const override;
    private:
        std::string GetLevelColor(spdlog::level::level_enum level);
    };

    class Log
    {
    public:
        static void Init(const std::string& name);
        static void Shutdown();
        static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
        static void SetLogLevel(spdlog::level::level_enum level);
        static void EnableFileLogging(const std::string& filename);

        // Internal logging functions that handle colored formatting
        template<typename... Args>
        static void LogTrace(const std::string& format, Args&&... args)
        {
            if (s_Logger) {
                std::string colored_msg = Format(format, std::forward<Args>(args)...);
                s_Logger->trace(colored_msg);
            }
        }

        template<typename... Args>
        static void LogInfo(const std::string& format, Args&&... args)
        {
            if (s_Logger) {
                std::string colored_msg = Format(format, std::forward<Args>(args)...);
                s_Logger->info(colored_msg);
            }
        }

        template<typename... Args>
        static void LogWarn(const std::string& format, Args&&... args)
        {
            if (s_Logger) {
                std::string colored_msg = Format(format, std::forward<Args>(args)...);
                s_Logger->warn(colored_msg);
            }
        }

        template<typename... Args>
        static void LogError(const std::string& format, Args&&... args)
        {
            if (s_Logger) {
                std::string colored_msg = Format(format, std::forward<Args>(args)...);
                s_Logger->error(colored_msg);
            }
        }

        template<typename... Args>
        static void LogCritical(const std::string& format, Args&&... args)
        {
            if (s_Logger) {
                std::string colored_msg = Format(format, std::forward<Args>(args)...);
                s_Logger->critical(colored_msg);
            }
        }

        // Overloads for simple string messages (no formatting)
        static void LogTrace(const std::string& message) { if (s_Logger) s_Logger->trace(LogColors::WHITE + message + LogColors::RESET); }
        static void LogInfo(const std::string& message) { if (s_Logger) s_Logger->info(LogColors::WHITE + message + LogColors::RESET); }
        static void LogWarn(const std::string& message) { if (s_Logger) s_Logger->warn(LogColors::WHITE + message + LogColors::RESET); }
        static void LogError(const std::string& message) { if (s_Logger) s_Logger->error(LogColors::WHITE + message + LogColors::RESET); }
        static void LogCritical(const std::string& message) { if (s_Logger) s_Logger->critical(LogColors::WHITE + message + LogColors::RESET); }

    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
        static std::vector<spdlog::sink_ptr> s_Sinks;
        static bool s_Initialized;
    };
}

#ifdef LUMINA_DEBUG
    #define LUMINA_LOG_TRACE(...) ::Lumina::Log::LogTrace(__VA_ARGS__)
    #define LUMINA_LOG_INFO(...) ::Lumina::Log::LogInfo(__VA_ARGS__)
    #define LUMINA_LOG_WARN(...) ::Lumina::Log::LogWarn(__VA_ARGS__)
    #define LUMINA_LOG_ERROR(...) ::Lumina::Log::LogError(__VA_ARGS__)
    #define LUMINA_LOG_CRITICAL(...) ::Lumina::Log::LogCritical(__VA_ARGS__)
#else
    #define LUMINA_LOG_TRACE(...) ((void)0)
    #define LUMINA_LOG_INFO(...) ((void)0)
    #define LUMINA_LOG_WARN(...) ((void)0)
    #define LUMINA_LOG_ERROR(...) ((void)0)
    #define LUMINA_LOG_CRITICAL(...) ((void)0)
#endif