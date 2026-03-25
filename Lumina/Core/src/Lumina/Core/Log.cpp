#include "Log.h"

namespace Lumina
{
	namespace Log
	{
		std::shared_ptr<spdlog::logger> Logger::s_Logger;
		std::vector<spdlog::sink_ptr> Logger::s_Sinks;
		bool Logger::s_Initialized = false;

		std::unique_ptr<spdlog::formatter> LogFormatter::clone() const
		{
			return std::make_unique<LogFormatter>();
		}

		void LogFormatter::format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest)
		{
			std::string levelColor = GetLevelColor(msg.level);
			std::string loggerName(msg.logger_name.data(), msg.logger_name.size());
			auto timeT = std::chrono::system_clock::to_time_t(msg.time);
			auto tm = spdlog::details::os::localtime(timeT);

			fmt::format_to(
				std::back_inserter(dest),
				"{}[{}{:02d}:{:02d}:{:02d}{}] "
				"{}[{}{}{}] "
				"{}[{}{}{}] "
				"{}{}{}\n",
				Colors::Reset, Colors::Gray, tm.tm_hour, tm.tm_min, tm.tm_sec, Colors::Reset,
				Colors::Reset, Colors::Orange, loggerName, Colors::Reset,
				Colors::Reset, levelColor, spdlog::level::to_string_view(msg.level), Colors::Reset,
				Colors::Gray, msg.payload, Colors::Reset
			);
		}

		std::string LogFormatter::GetLevelColor(spdlog::level::level_enum level)
		{
			switch (level)
			{
			case spdlog::level::trace:    return Colors::Cyan;
			case spdlog::level::debug:    return Colors::Reset;
			case spdlog::level::info:     return Colors::Green;
			case spdlog::level::warn:     return Colors::Yellow;
			case spdlog::level::err:      return Colors::Red;
			case spdlog::level::critical: return Colors::BoldRed;
			default:                      return Colors::Reset;
			}
		}

		void Logger::Init(const std::string& name)
		{
			if (s_Initialized)
				return;

			auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
			consoleSink->set_formatter(std::make_unique<LogFormatter>());
			s_Sinks.push_back(consoleSink);

			s_Logger = std::make_shared<spdlog::logger>(name, s_Sinks.begin(), s_Sinks.end());
			s_Logger->set_level(spdlog::level::trace);
			s_Logger->flush_on(spdlog::level::err);
			spdlog::register_logger(s_Logger);

			s_Initialized = true;
			Logger::Info("Log: Initialization complete");
		}

		void Logger::Shutdown()
		{
			if (!s_Initialized)
				return;

			Logger::Info("Log: Shutting down...");
			if (s_Logger)
				s_Logger->flush();

			spdlog::shutdown();
			s_Initialized = false;
		}

		void Logger::SetName(const std::string& name)
		{
			if (!s_Initialized || !s_Logger)
				return;

			// Unregister old logger
			spdlog::drop(s_Logger->name());

			// Create new logger with same sinks but new name
			auto level = s_Logger->level();
			s_Logger = std::make_shared<spdlog::logger>(name, s_Sinks.begin(), s_Sinks.end());
			s_Logger->set_level(level);
			s_Logger->flush_on(spdlog::level::err);
			spdlog::register_logger(s_Logger);
		}

		void Logger::SetLevel(spdlog::level::level_enum level)
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
				auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, 5 * 1024 * 1024, 3);
				fileSink->set_pattern("[%T] [%n] [%l] %v");
				s_Sinks.push_back(fileSink);
				s_Logger->sinks().push_back(fileSink);
				Logger::Info("File logging enabled: {}", filename);
			}
			catch (const std::exception& ex)
			{
				Logger::Error("Failed to enable file logging: {}", ex.what());
			}
		}
	}
}
