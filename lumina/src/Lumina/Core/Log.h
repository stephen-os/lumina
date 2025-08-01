#pragma once

#include <memory>
#include <string>
#include <chrono>

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
        static void Init(std::string& name);
        static void Shutdown();
        static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
        static void SetLogLevel(spdlog::level::level_enum level);
        static void EnableFileLogging(const std::string& filename);

        static std::string Format(const std::string& format);
    private:
        static std::shared_ptr<spdlog::logger> s_Logger;
        static std::vector<spdlog::sink_ptr> s_Sinks;
        static bool s_Initialized;
    };
}

#ifdef LUMINA_DEBUG
    #define LUMINA_LOG_TRACE(format, ...)    ::Lumina::Log::GetLogger()->trace(::Lumina::Log::Format(format), __VA_ARGS__)
    #define LUMINA_LOG_INFO(format, ...)     ::Lumina::Log::GetLogger()->info(::Lumina::Log::Format(format), __VA_ARGS__)
    #define LUMINA_LOG_WARN(format, ...)     ::Lumina::Log::GetLogger()->warn(::Lumina::Log::Format(format), __VA_ARGS__)
    #define LUMINA_LOG_ERROR(format, ...)    ::Lumina::Log::GetLogger()->error(::Lumina::Log::Format(format), __VA_ARGS__)
    #define LUMINA_LOG_CRITICAL(format, ...) ::Lumina::Log::GetLogger()->critical(::Lumina::Log::Format(format), __VA_ARGS__)
#else
    #define LUMINA_LOG_TRACE(...)    (void)0
    #define LUMINA_LOG_INFO(...)     (void)0
    #define LUMINA_LOG_WARN(...)     (void)0
    #define LUMINA_LOG_ERROR(...)    (void)0
    #define LUMINA_LOG_CRITICAL(...) (void)0
#endif