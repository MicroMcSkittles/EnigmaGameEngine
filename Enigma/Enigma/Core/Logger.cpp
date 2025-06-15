#include "Core/Logger.h"
#include "Core/System.h"
#include <iostream>
#include <sstream>
#include <math.h>
#include <iomanip>

namespace Enigma {
	namespace Core {

		void Logger::Init(const LoggerConfig& config) {
			if (s_Config) {
				std::cout << "Logger already initalized" << std::endl;
				return;
			}
			s_Config = new LoggerConfig(config);
		}

		void Logger::Log(const std::string& message, uint32_t priority, LogType type, const LogInfo& info) {
			if (priority > s_Config->priorityLevel) return;

			std::stringstream ss;
			switch (type)
			{
			case Enigma::Core::LogType::Error: ss << "Error "; break;
			case Enigma::Core::LogType::SoftError: ss << "Soft Error "; break;
			case Enigma::Core::LogType::Warning: ss << "Warning "; break;
			case Enigma::Core::LogType::Message:
			default: ss << "Message "; break;
			}

			std::string filename = std::string(info.file.begin() + info.file.find_last_of("\\") + 1, info.file.end());

			if((s_Config->flags & LoggerFile) && (s_Config->flags & LoggerLine))
				ss << "from file ( " << filename << " ) at line ( " << info.line << " ) ";
			else if (s_Config->flags & LoggerFile)
				ss << "from file ( " << filename << " ) ";
			else if (s_Config->flags & LoggerLine)
				ss << "from line ( " << info.line << " ) ";
			if (s_Config->flags & LoggerFunction)
				ss << "in function ( " << info.function << " ) ";

			if (s_Config->flags & LoggerPriority)
				ss << "with priority level " << priority;
			ss << std::endl;

			if(s_Config->flags & LoggerDate) {
				Time t = System::GetTime();
				int year = 1900 + t.year;
				ss << t.dayName << " ( " << t.month << " / " << t.day << " / " << year << " )" << std::endl;
			}
			if(s_Config->flags & LoggerTime) {
				Time t = System::GetTime();
				bool PM = t.hours > 12;
				int hours = t.hours - (PM? 12 : 0);

				ss << hours << " : ";
				ss << std::right << std::setw(2) << std::setfill('0') << t.minutes;
				ss << " " << ((PM)? "PM" : "AM") << std::endl;
			}

			ss << "\"" << std::endl;
			ss << message << std::endl;
			ss << "\"" << std::endl;

			std::cout << ss.str() << std::endl;

			if (type == LogType::Error) exit(-1);
		}

	}
}