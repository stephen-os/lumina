#include "log.h"

namespace lumina::core
{
    namespace log
    {
        std::shared_ptr<spdlog::logger> logger::s_logger;
        std::vector<spdlog::sink_ptr> logger::s_sinks;
        bool logger::s_initialized = false;

        std::unique_ptr<spdlog::formatter> log_formatter::clone() const
        {
            return std::make_unique<log_formatter>();
        }

        void log_formatter::format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest)
        {
            std::string level_color = get_level_color(msg.level);
            std::string logger_name(msg.logger_name.data(), msg.logger_name.size());
            auto time_t = std::chrono::system_clock::to_time_t(msg.time);
            auto tm = spdlog::details::os::localtime(time_t);

            fmt::format_to(
                std::back_inserter(dest),
                "{}[{}{:02d}:{:02d}:{:02d}{}] "
                "{}[{}{}{}] "
                "{}[{}{}{}] "
                "{}{}{}\n",
                colors::reset, colors::gray, tm.tm_hour, tm.tm_min, tm.tm_sec, colors::reset,
                colors::reset, colors::orange, logger_name, colors::reset,
                colors::reset, level_color, spdlog::level::to_string_view(msg.level), colors::reset,
                colors::gray, msg.payload, colors::reset
            );
        }

        std::string log_formatter::get_level_color(spdlog::level::level_enum level)
        {
            switch (level)
            {
            case spdlog::level::trace:    return colors::cyan;
            case spdlog::level::debug:    return colors::reset;
            case spdlog::level::info:     return colors::green;
            case spdlog::level::warn:     return colors::yellow;
            case spdlog::level::err:      return colors::red;
            case spdlog::level::critical: return colors::bold_red;
            default:                      return colors::reset;
            }
        }

        void logger::init(const std::string& name)
        {
            if (s_initialized)
                return;

            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_formatter(std::make_unique<log_formatter>());
            s_sinks.push_back(console_sink);

            s_logger = std::make_shared<spdlog::logger>(name, s_sinks.begin(), s_sinks.end());
            s_logger->set_level(spdlog::level::trace);
            s_logger->flush_on(spdlog::level::err);
            spdlog::register_logger(s_logger);

            s_initialized = true;
            logger::info("Log: Initialization complete");
        }

        void logger::shutdown()
        {
            if (!s_initialized)
                return;

            logger::info("Log: Shutting down...");
            if (s_logger)
                s_logger->flush();

            spdlog::shutdown();
            s_initialized = false;
        }

        void logger::set_name(const std::string& name)
        {
            if (!s_initialized || !s_logger)
                return;

            // Unregister old logger
            spdlog::drop(s_logger->name());

            // Create new logger with same sinks but new name
            auto level = s_logger->level();
            s_logger = std::make_shared<spdlog::logger>(name, s_sinks.begin(), s_sinks.end());
            s_logger->set_level(level);
            s_logger->flush_on(spdlog::level::err);
            spdlog::register_logger(s_logger);
        }

        void logger::set_level(spdlog::level::level_enum level)
        {
            if (!s_initialized || !s_logger)
                return;

            s_logger->set_level(level);
        }

        void logger::enable_file_logging(const std::string& filename)
        {
            if (!s_initialized || !s_logger)
                return;

            try
            {
                auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, 5 * 1024 * 1024, 3);
                file_sink->set_pattern("[%T] [%n] [%l] %v");
                s_sinks.push_back(file_sink);
                s_logger->sinks().push_back(file_sink);
                logger::info("File logging enabled: {}", filename);
            }
            catch (const std::exception& ex)
            {
                logger::error("Failed to enable file logging: {}", ex.what());
            }
        }
    }
}
