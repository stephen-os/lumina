#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <sstream>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace lumina::core
{
    namespace log
    {
        namespace colors
        {
            constexpr const char* reset = "\033[0m";
            constexpr const char* gray = "\033[90m";
            constexpr const char* green = "\033[32m";
            constexpr const char* yellow = "\033[33m";
            constexpr const char* red = "\033[31m";
            constexpr const char* cyan = "\033[36m";
            constexpr const char* orange = "\033[38;5;208m";
            constexpr const char* bold_red = "\033[1;31m";
        }

        class log_formatter : public spdlog::formatter
        {
        public:
            log_formatter() = default;
            void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override;
            std::unique_ptr<spdlog::formatter> clone() const override;

        private:
            std::string get_level_color(spdlog::level::level_enum level);
        };

        class logger
        {
        public:
            static void init(const std::string& name);
            static void shutdown();
            static void set_name(const std::string& name);
            static std::shared_ptr<spdlog::logger>& get() { return s_logger; }
            static void set_level(spdlog::level::level_enum level);
            static void enable_file_logging(const std::string& filename);

            template<typename... Args>
            static void trace(const std::string& format, Args&&... args)
            {
                if (s_logger) s_logger->trace(fmt::runtime(format), std::forward<Args>(args)...);
            }

            template<typename... Args>
            static void info(const std::string& format, Args&&... args)
            {
                if (s_logger) s_logger->info(fmt::runtime(format), std::forward<Args>(args)...);
            }

            template<typename... Args>
            static void warn(const std::string& format, Args&&... args)
            {
                if (s_logger) s_logger->warn(fmt::runtime(format), std::forward<Args>(args)...);
            }

            template<typename... Args>
            static void error(const std::string& format, Args&&... args)
            {
                if (s_logger) s_logger->error(fmt::runtime(format), std::forward<Args>(args)...);
            }

            template<typename... Args>
            static void critical(const std::string& format, Args&&... args)
            {
                if (s_logger) s_logger->critical(fmt::runtime(format), std::forward<Args>(args)...);
            }

            static void trace(const std::string& message)
            {
                if (s_logger) s_logger->trace(message);
            }

            static void info(const std::string& message)
            {
                if (s_logger) s_logger->info(message);
            }

            static void warn(const std::string& message)
            {
                if (s_logger) s_logger->warn(message);
            }

            static void error(const std::string& message)
            {
                if (s_logger) s_logger->error(message);
            }

            static void critical(const std::string& message)
            {
                if (s_logger) s_logger->critical(message);
            }

        private:
            static std::shared_ptr<spdlog::logger> s_logger;
            static std::vector<spdlog::sink_ptr> s_sinks;
            static bool s_initialized;
        };

        inline void init(const std::string& name) { logger::init(name); }
        inline void shutdown() { logger::shutdown(); }
        inline void set_name(const std::string& name) { logger::set_name(name); }
        inline void set_level(spdlog::level::level_enum level) { logger::set_level(level); }
        inline void enable_file_logging(const std::string& filename) { logger::enable_file_logging(filename); }
    }
}

#ifdef LUMINA_DEBUG
    #define LUMINA_LOG_TRACE(...) ::lumina::core::log::logger::trace(__VA_ARGS__)
    #define LUMINA_LOG_INFO(...) ::lumina::core::log::logger::info(__VA_ARGS__)
    #define LUMINA_LOG_WARN(...) ::lumina::core::log::logger::warn(__VA_ARGS__)
    #define LUMINA_LOG_ERROR(...) ::lumina::core::log::logger::error(__VA_ARGS__)
    #define LUMINA_LOG_CRITICAL(...) ::lumina::core::log::logger::critical(__VA_ARGS__)
#else
    #define LUMINA_LOG_TRACE(...) ((void)0)
    #define LUMINA_LOG_INFO(...) ((void)0)
    #define LUMINA_LOG_WARN(...) ((void)0)
    #define LUMINA_LOG_ERROR(...) ((void)0)
    #define LUMINA_LOG_CRITICAL(...) ((void)0)
#endif
