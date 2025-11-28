#include "Log.h"

namespace Lumina::Core
{
    namespace Log
    {
        std::shared_ptr<spdlog::logger> Logger::s_Logger;
        std::vector<spdlog::sink_ptr> Logger::s_Sinks;
        bool Logger::s_Initialized = false;

        std::unique_ptr<spdlog::formatter> LoggerFormatter::clone() const
        {
            return std::make_unique<LoggerFormatter>();
        }

        void LoggerFormatter::format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest)
        {
            std::string level_color = GetLevelColor(msg.level);
            std::string logger_name(msg.logger_name.data(), msg.logger_name.size());
            auto time_t = std::chrono::system_clock::to_time_t(msg.time);
            auto tm = spdlog::details::os::localtime(time_t);

            // Format: [Time] [Application Name] [Level] Message
            fmt::format_to(
                std::back_inserter(dest),                                                                   
                "{}[{}{:02d}:{:02d}:{:02d}{}] " // Time
                "{}[{}{}{}] "                   // Application Name 
                "{}[{}{}{}] "                   // Level
                "{}{}{}\n",                     // Message
                Colors::Default, Colors::Gray, tm.tm_hour, tm.tm_min, tm.tm_sec, Colors::Default,           
                Colors::Default, Colors::Orange, logger_name, Colors::Default,                              
                Colors::Default, level_color, spdlog::level::to_string_view(msg.level), Colors::Default,    
                Colors::Gray, msg.payload, Colors::Default                                                
            );
        }

        std::string LoggerFormatter::GetLevelColor(spdlog::level::level_enum level)
        {
            switch (level)
            {
            case spdlog::level::trace:    return Colors::Cyan;
            case spdlog::level::debug:    return Colors::Default;
            case spdlog::level::info:     return Colors::Green;
            case spdlog::level::warn:     return Colors::Yellow;
            case spdlog::level::err:      return Colors::Red;
            case spdlog::level::critical: return Colors::BoldRed;
            default:                      return Colors::Default;
            }
        }

        void Logger::Init(const std::string& name)
        {
            if (s_Initialized)
                return;

            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_formatter(std::make_unique<LoggerFormatter>());
            s_Sinks.push_back(console_sink);

            s_Logger = std::make_shared<spdlog::logger>(name, s_Sinks.begin(), s_Sinks.end());
            s_Logger->set_level(spdlog::level::trace);
            s_Logger->flush_on(spdlog::level::err);
            spdlog::register_logger(s_Logger);

            s_Initialized = true;
            Logger::LogInfo("Log: Initialization complete");
        }

        void Logger::Shutdown()
        {
            if (!s_Initialized)
                return;

            Logger::LogInfo("Log: Shutting down...");
            if (s_Logger)
                s_Logger->flush();

            spdlog::shutdown();
            s_Initialized = false;
        }

        void Logger::SetLogLevel(spdlog::level::level_enum level)
        {
            if (!s_Initialized || !s_Logger)
                return;

            s_Logger->set_level(level);
        }

        void Logger::EnableFileLogging(const std::string& filename)
        {
            if (!s_Initialized || !s_Logger)
                return;

            try
            {
                auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, 5 * 1024 * 1024, 3);
                file_sink->set_pattern("[%T] [%n] [%l] %v");  // Plain format for files
                s_Sinks.push_back(file_sink);
                s_Logger->sinks().push_back(file_sink);
                Logger::LogInfo("File logging enabled: {}", filename);
            }
            catch (const std::exception& ex)
            {
                Logger::LogError("Failed to enable file logging: {}", ex.what());
            }
        }
    }
}