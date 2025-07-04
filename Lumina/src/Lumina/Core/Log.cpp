#include "Log.h"

namespace Lumina
{
    std::shared_ptr<spdlog::logger> Log::s_Logger;
    std::vector<spdlog::sink_ptr> Log::s_Sinks;
    bool Log::s_Initialized = false;

    std::unique_ptr<spdlog::formatter> LogFormatter::clone() const
    {
        return std::make_unique<LogFormatter>();
    }

    void LogFormatter::format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest)
    {
        std::string level_color = GetLevelColor(msg.level);
        std::string logger_name(msg.logger_name.data(), msg.logger_name.size());

        auto time_t = std::chrono::system_clock::to_time_t(msg.time);
        auto tm = spdlog::details::os::localtime(time_t);

        fmt::format_to(std::back_inserter(dest),
            "{}[{}{:02d}:{:02d}:{:02d}{}] "         // [TIME] - brackets white, time gray
            "{}[{}{}{}] "                           // [LOGGER] - brackets white, logger green
            "{}[{}{}{}] "                           // [LEVEL] - brackets white, level colored
            "{}{}{}\n",                             // MESSAGE - white text + newline
            LogColors::WHITE, LogColors::GRAY, tm.tm_hour, tm.tm_min, tm.tm_sec, LogColors::WHITE,
            LogColors::WHITE, LogColors::ORANGE, logger_name, LogColors::WHITE,
            LogColors::WHITE, level_color, spdlog::level::to_string_view(msg.level).data(), LogColors::WHITE,
            LogColors::WHITE, fmt::to_string(msg.payload), LogColors::RESET);
    }

    std::string LogFormatter::GetLevelColor(spdlog::level::level_enum level)
    {
        switch (level)
        {
        case spdlog::level::trace:    return LogColors::CYAN;
        case spdlog::level::debug:    return LogColors::WHITE;
        case spdlog::level::info:     return LogColors::GREEN;
        case spdlog::level::warn:     return LogColors::YELLOW;
        case spdlog::level::err:      return LogColors::RED;
        case spdlog::level::critical: return LogColors::BOLD_RED;
        default:                      return LogColors::WHITE;
        }
    }

    void Log::Init()
    {
        if (s_Initialized)
            return;

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_formatter(std::make_unique<LogFormatter>());
        s_Sinks.push_back(console_sink);

        s_Logger = std::make_shared<spdlog::logger>("Lumina", s_Sinks.begin(), s_Sinks.end());
        s_Logger->set_level(spdlog::level::trace);
        s_Logger->flush_on(spdlog::level::err);
        spdlog::register_logger(s_Logger);

        s_Initialized = true;

        LUMINA_LOG_INFO("Logging system initialized");
    }

    void Log::Shutdown()
    {
        if (!s_Initialized)
            return;

        if (s_Logger)
            s_Logger->flush();

        spdlog::shutdown();
        s_Initialized = false;
    }

    void Log::SetLogLevel(spdlog::level::level_enum level)
    {
        if (!s_Initialized || !s_Logger)
            return;

        s_Logger->set_level(level);
    }

    void Log::EnableFileLogging(const std::string& filename)
    {
        if (!s_Initialized || !s_Logger)
            return;

        try
        {
            auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, 5 * 1024 * 1024, 3);
            file_sink->set_pattern("[%T] [%n] [%l] %v");  // Plain format for files
            s_Sinks.push_back(file_sink);

            s_Logger->sinks().push_back(file_sink);

            LUMINA_LOG_INFO("File logging enabled: {}", filename);
        }
        catch (const std::exception& ex)
        {
            LUMINA_LOG_ERROR("Failed to enable file logging: {}", ex.what());
        }
    }

    std::string Log::Format(const std::string& format)
    {
        std::string result = format;
        size_t pos = 0;

        while ((pos = result.find("{}", pos)) != std::string::npos) 
        {
            std::string replacement = std::string(LogColors::YELLOW) + "{}" + LogColors::WHITE;
            result.replace(pos, 2, replacement);
            pos += replacement.length();
        }

        pos = 0;
        while ((pos = result.find("{", pos)) != std::string::npos) 
        {
            size_t end_pos = result.find("}", pos);
            if (end_pos != std::string::npos) 
            {
                std::string placeholder = result.substr(pos, end_pos - pos + 1);
                if (placeholder.find("\033[") == std::string::npos) 
                {
                    std::string replacement = std::string(LogColors::YELLOW) + placeholder + LogColors::WHITE;
                    result.replace(pos, end_pos - pos + 1, replacement);
                    pos += replacement.length();
                }
                else 
                {
                    pos = end_pos + 1;
                }
            }
            else 
            {
                break;
            }
        }

        return result;
    }
}