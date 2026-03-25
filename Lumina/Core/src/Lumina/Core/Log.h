#pragma once

#include <memory>
#include <string>
#include <chrono>
#include <sstream>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace Lumina
{
	namespace Log
	{
		namespace Colors
		{
			constexpr const char* Reset = "\033[0m";
			constexpr const char* Gray = "\033[90m";
			constexpr const char* Green = "\033[32m";
			constexpr const char* Yellow = "\033[33m";
			constexpr const char* Red = "\033[31m";
			constexpr const char* Cyan = "\033[36m";
			constexpr const char* Orange = "\033[38;5;208m";
			constexpr const char* BoldRed = "\033[1;31m";
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

		class Logger
		{
		public:
			static void Init(const std::string& name);
			static void Shutdown();
			static void SetName(const std::string& name);
			static std::shared_ptr<spdlog::logger>& Get() { return s_Logger; }
			static void SetLevel(spdlog::level::level_enum level);
			static void EnableFileLogging(const std::string& filename);

			template<typename... Args>
			static void Trace(const std::string& format, Args&&... args)
			{
				if (s_Logger) s_Logger->trace(fmt::runtime(format), std::forward<Args>(args)...);
			}

			template<typename... Args>
			static void Info(const std::string& format, Args&&... args)
			{
				if (s_Logger) s_Logger->info(fmt::runtime(format), std::forward<Args>(args)...);
			}

			template<typename... Args>
			static void Warn(const std::string& format, Args&&... args)
			{
				if (s_Logger) s_Logger->warn(fmt::runtime(format), std::forward<Args>(args)...);
			}

			template<typename... Args>
			static void Error(const std::string& format, Args&&... args)
			{
				if (s_Logger) s_Logger->error(fmt::runtime(format), std::forward<Args>(args)...);
			}

			template<typename... Args>
			static void Critical(const std::string& format, Args&&... args)
			{
				if (s_Logger) s_Logger->critical(fmt::runtime(format), std::forward<Args>(args)...);
			}

			static void Trace(const std::string& message)
			{
				if (s_Logger) s_Logger->trace(message);
			}

			static void Info(const std::string& message)
			{
				if (s_Logger) s_Logger->info(message);
			}

			static void Warn(const std::string& message)
			{
				if (s_Logger) s_Logger->warn(message);
			}

			static void Error(const std::string& message)
			{
				if (s_Logger) s_Logger->error(message);
			}

			static void Critical(const std::string& message)
			{
				if (s_Logger) s_Logger->critical(message);
			}

		private:
			static std::shared_ptr<spdlog::logger> s_Logger;
			static std::vector<spdlog::sink_ptr> s_Sinks;
			static bool s_Initialized;
		};

		inline void Init(const std::string& name) { Logger::Init(name); }
		inline void Shutdown() { Logger::Shutdown(); }
		inline void SetName(const std::string& name) { Logger::SetName(name); }
		inline void SetLevel(spdlog::level::level_enum level) { Logger::SetLevel(level); }
		inline void EnableFileLogging(const std::string& filename) { Logger::EnableFileLogging(filename); }
	}
}

#ifdef LUMINA_DEBUG
	#define LUMINA_LOG_TRACE(...) ::Lumina::Log::Logger::Trace(__VA_ARGS__)
	#define LUMINA_LOG_INFO(...) ::Lumina::Log::Logger::Info(__VA_ARGS__)
	#define LUMINA_LOG_WARN(...) ::Lumina::Log::Logger::Warn(__VA_ARGS__)
	#define LUMINA_LOG_ERROR(...) ::Lumina::Log::Logger::Error(__VA_ARGS__)
	#define LUMINA_LOG_CRITICAL(...) ::Lumina::Log::Logger::Critical(__VA_ARGS__)
#else
	#define LUMINA_LOG_TRACE(...) ((void)0)
	#define LUMINA_LOG_INFO(...) ((void)0)
	#define LUMINA_LOG_WARN(...) ((void)0)
	#define LUMINA_LOG_ERROR(...) ((void)0)
	#define LUMINA_LOG_CRITICAL(...) ((void)0)
#endif
