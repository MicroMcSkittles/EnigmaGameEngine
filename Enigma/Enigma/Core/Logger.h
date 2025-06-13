#pragma once
#include <stdint.h>
#include <string>

namespace Enigma {
	namespace Core {
		// Tells to logger what info to include in logs
		enum LoggerFlag {
			None = 0,
			Time     = 1 << 1, // Not impl yet
			Date     = 2 << 1, // Not impl yet
			Priority = 3 << 1,

			// Implemented in Core.h

			Function = 4 << 1, 
			Line     = 5 << 1,
			File     = 6 << 1,
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
			uint32_t priorityLevel = 5; // Tells the logger what to keep and ignore

			// For when I add the ability to save logs to a file
			bool saveToFile = false; // Tells the logger to save logs to a file.
			std::string path = "";
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