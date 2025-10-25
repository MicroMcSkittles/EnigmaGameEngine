#pragma once
#include <string>
#include <functional>
#include <iostream>

#include "Enigma/Core/Types.h"

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

		struct LogInfo {
			LogType type;
			u8 priority;
			std::string file;
			std::string function;
			long line;
		};

		struct LoggerConfig {
			u32 flags;
			u32 priorityLevel = 5; // Tells the logger what to keep and ignore, the higher the number the lower the priority

			// For when I add the ability to save logs to a file
			bool saveToFile = false; // Tells the logger to save logs to a file.
			std::string path = "";

			std::function<void(const std::string&, const LogInfo&)> logCallback = [](const std::string& message, const LogInfo& info) {
				if (info.type == LogType::Error) std::cout << "\033[31m";
				else if (info.type == LogType::SoftError) std::cout << "\033[31m";
				else if (info.type == LogType::Warning) std::cout << "\033[33m";
				else if (info.type == LogType::Message) std::cout << "\033[32m";
				std::cout << message << std::endl;
				std::cout << "\033[0m";
				if (info.type == LogType::Error) exit(-1);
			};
		};

		class Logger {
		public:
			static void Init(const LoggerConfig& config);
			static unique<LoggerConfig>& GetConfig() { return s_Config; }

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
			inline static unique<LoggerConfig> s_Config;
		};
	}
};