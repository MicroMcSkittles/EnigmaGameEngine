#pragma once
#include <stdint.h>
#include <string>

namespace Enigma {
	namespace Core {
		// Tells to logger what info to include in logs
		enum LoggerFlag {
			None = 0,
			LoggerTime     = 1 << 1,
			LoggerDate     = 1 << 2,
			LoggerPriority = 1 << 3,

			LoggerFunction = 1 << 4, 
			LoggerLine     = 1 << 5,
			LoggerFile     = 1 << 6,

			// Thiner but 5 lines tall minimum
			LoggerTall	   = 1 << 7,
			// Wider and 1 line tall minimum
			LoggerShort    = 1 << 8,
		};
		enum class LogType {
			None = 0,
			Error,
			SoftError,
			Warning,
			Message
		};

		struct LoggerConfig {
			uint32_t flags;
			uint32_t priorityLevel = 5; // Tells the logger what to keep and ignore, the higher the number the lower the priority

			// For when I add the ability to save logs to a file
			bool saveToFile = false; // Tells the logger to save logs to a file.
			std::string path = "";

			LoggerConfig(uint32_t flags = LoggerFile | LoggerFunction | LoggerPriority,
				uint32_t priorityLevel = 5, bool saveToFile = false, 
				const std::string& path = "")
				: flags(flags), priorityLevel(priorityLevel), saveToFile(saveToFile), path(path) { }
		};

		struct LogInfo {
			LogType type;
			uint8_t priority;
			std::string file;
			std::string function;
			long line;
		};

		class Logger {
		public:
			static void Init(const LoggerConfig& config);
			static LoggerConfig* GetConfig() { return s_Config; }

			// Proccesses all logs
			static void Log(const std::string& message, LogInfo info, ...);

			static std::string TallLog(const std::string& message, const LogInfo& info);
			static std::string ShortLog(const std::string& message, const LogInfo& info);

		private:
			// I have a irrational hate for any C++ version past C++17 so
			// you get this format function, use it the same way you would printf
			static std::string Format(std::string str, va_list args);
			static std::string Format(std::string str, ...);

		private:
			inline static LoggerConfig* s_Config;
		};
	}
};