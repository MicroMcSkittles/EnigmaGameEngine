#include "Core/Logger.h"
#include <iostream>
#include <sstream>

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

			if((s_Config->flags & LoggerFlag::File) && (s_Config->flags & LoggerFlag::Line))
				ss << "from file ( " << filename << " ) at line ( " << info.line << " ) ";
			else if (s_Config->flags & LoggerFlag::File)
				ss << "from file ( " << filename << " ) ";
			else if (s_Config->flags & LoggerFlag::Line)
				ss << "from line ( " << info.line << " ) ";
			if (s_Config->flags & LoggerFlag::Function)
				ss << "in function ( " << info.function << " )";

			ss << std::endl;
			if (s_Config->flags & LoggerFlag::Priority)
				ss << "Priority " << priority << std::endl;

			ss << "\"" << std::endl;
			ss << message << std::endl;
			ss << "\"" << std::endl;

			std::cout << ss.str() << std::endl;

			if (type == LogType::Error) exit(-1);
		}

	}
}