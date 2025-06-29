#include "Core/Utilities/Logger.h"
#include "Core/System.h"
#include <iostream>
#include <sstream>
#include <math.h>
#include <iomanip>
#include <cstdarg>
#include <vector>

namespace Enigma {
	namespace Core {

		void Logger::Init(const LoggerConfig& config) {
			if (s_Config) {
				std::cout << "Logger already initalized" << std::endl;
				return;
			}
			s_Config = new LoggerConfig(config);
		}

		void Logger::Log(const std::string& message, LogInfo info, ...) {
			if (info.priority > s_Config->priorityLevel) return;

			va_list args;
			va_start(args, info);
			std::string msg = Format(message, args);
			va_end(args);

			std::string log;
			if (s_Config->flags & LoggerTall) log = TallLog(msg, info);
			else if (s_Config->flags & LoggerShort) log = ShortLog(msg, info);

			if (info.type == LogType::Error) std::cout << "\033[31m";
			else if (info.type == LogType::SoftError) std::cout << "\033[31m";
			else if (info.type == LogType::Warning) std::cout << "\033[33m";
			else if (info.type == LogType::Message) std::cout << "\033[32m";
			std::cout << log << std::endl;
			std::cout << "\033[0m";

			if (info.type == LogType::Error) exit(-1);
		}

		std::string Logger::TallLog(const std::string& message, const LogInfo& info)
		{
			std::stringstream ss;
			switch (info.type)
			{
			case Enigma::Core::LogType::Error: ss << "Error "; break;
			case Enigma::Core::LogType::SoftError: ss << "Soft Error "; break;
			case Enigma::Core::LogType::Warning: ss << "Warning "; break;
			case Enigma::Core::LogType::Message:
			default: ss << "Message "; break;
			}

			std::string filename = std::string(info.file.begin() + info.file.find_last_of("\\") + 1, info.file.end());

			if ((s_Config->flags & LoggerFile) && (s_Config->flags & LoggerLine))
				ss << "from file ( " << filename << " ) at line ( " << info.line << " ) ";
			else if (s_Config->flags & LoggerFile)
				ss << "from file ( " << filename << " ) ";
			else if (s_Config->flags & LoggerLine)
				ss << "from line ( " << info.line << " ) ";
			if (s_Config->flags & LoggerFunction)
				ss << "in function ( " << info.function << " ) ";

			if (s_Config->flags & LoggerPriority)
				ss << "with priority level " << info.priority;
			ss << std::endl;

			if (s_Config->flags & LoggerDate) {
				Time t = System::GetTime();
				int year = 1900 + t.year;
				ss << t.dayName << " ( " << t.month << " / " << t.day << " / " << year << " )" << std::endl;
			}
			if (s_Config->flags & LoggerTime) {
				Time t = System::GetTime();
				ss << t.ToString() << std::endl;
			}

			ss << "\"" << std::endl;
			ss << message << std::endl;
			ss << "\"" << std::endl;

			return ss.str();
		}

		std::string Logger::ShortLog(const std::string& message, const LogInfo& info)
		{
			std::stringstream ss;

			if (s_Config->flags & LoggerTime) {
				Time t = System::GetTime();
				ss << "[" << t.ToString() << "] ";
			}
			if (s_Config->flags & LoggerDate) {
				Time t = System::GetTime();
				int year = 1900 + t.year;
				ss << "[" << t.month << "/" << t.day << "/" << year << "] ";
			}
			if (s_Config->flags & LoggerFile) {
				std::string filename = std::string(info.file.begin() + info.file.find_last_of("\\") + 1, info.file.end());
				ss << "[" << filename << "] ";
			}
			if (s_Config->flags & LoggerFunction) {
				ss << "[" << info.function << "] ";
			}
			if (s_Config->flags & LoggerLine) {
				ss << "[Line: " << info.line << "] ";
			}
			if (s_Config->flags & LoggerPriority) {
				ss << "[" << (int)info.priority << "] ";
			}
			ss << message;
			return ss.str();
		}

		std::string Logger::Format(std::string str, va_list args)
		{

			std::stringstream formated;

			size_t lastPosition = 0;
			size_t position = 0;
			int index = 0;
			while ((position = str.find_first_of("%", position)) != std::string::npos) {

				if(position != 0) 
					formated << std::string(str.begin() + lastPosition, str.begin() + position);

				if (position + 1 == str.size()) break;

				char type = str[position + 1];

				switch (type)
				{
				case '%': formated << '%'; break;
				case 'c': formated << va_arg(args, char); break;

				case 'd':
				case 'i': formated << va_arg(args, int); break;

				case 'u': formated << va_arg(args, uint32_t); break;

				case 'x':
				case 'X': formated << std::hex << va_arg(args, uint32_t); break;

				case 'f': formated << va_arg(args, float); break;

				case 's': formated << va_arg(args, const char*); break;
				}

				index += 1;
				position += 2;
				lastPosition = position;
			}

			formated << std::string(str.begin() + lastPosition, str.end());

			return formated.str();
		}

		std::string Logger::Format(std::string str, ...)
		{
			va_list args;
			va_start(args, str);
			std::string msg = Format(str, args);
			va_end(args);
			return msg;
		}

	}
}