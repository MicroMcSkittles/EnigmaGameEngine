#pragma once
#include <stdint.h>
#include <string>

namespace Enigma {
	namespace Core {
		// Tells to logger what info to include in logs
		enum LoggerFlag {
			None = 0,
			LoggerTime     = 1 << 1, // Not impl yet
			LoggerDate     = 1 << 2, // Not impl yet
			LoggerPriority = 1 << 3,

			// Implemented in Core.h

			LoggerFunction = 1 << 4, 
			LoggerLine     = 1 << 5,
			LoggerFile     = 1 << 6,
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
			std::string file;
			std::string function;
			long line;
		};

		class Logger {
		public:
			static void Init(const LoggerConfig& config);
			static LoggerConfig* GetConfig() { return s_Config; }

			// Proccesses all logs
			static void Log(const std::string& message, uint32_t priority, LogType type, const LogInfo& info);

		private:
			inline static LoggerConfig* s_Config;
		};
	}
};