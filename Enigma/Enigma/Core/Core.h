#pragma once
#include <cstdlib>

#pragma region Disable Warnings
// Disable the "Enum type is unscoped" warning
#pragma warning(disable : 26812)
// Disable the "___ isn't initalized" warning
#pragma warning(disable : 26495)

#pragma endregion

#pragma region utils

#define BIT(x) 1 << x

#pragma endregion

#pragma region Logger Impl
#ifdef DEBUG
#define ENABLE_LOGGER
#endif

#ifdef ENABLE_LOGGER

#include "Core/Utilities/Logger.h"

#define INIT_LOGGER(config) Enigma::Core::Logger::Init(config)


#define LOG_ERROR(msg) Enigma::Core::Logger::Log(msg, 0, Enigma::Core::LogType::Error, { __FILE__, __func__, __LINE__  })
#define LOG_SOFT_ERROR(msg) Enigma::Core::Logger::Log(msg, 0, Enigma::Core::LogType::SoftError, { __FILE__, __func__, __LINE__  })
#define LOG_WARNING(msg) Enigma::Core::Logger::Log(msg, 1, Enigma::Core::LogType::Warning, { __FILE__, __func__, __LINE__  })
#define LOG_MESSAGE(msg, priority) Enigma::Core::Logger::Log(msg, priority, Enigma::Core::LogType::Message, { __FILE__, __func__, __LINE__  })

#define LOG_ASSERT(condition, msg) if(condition) LOG_ERROR(msg);

#else

#define INIT_LOGGER(config)

#define LOG_ERROR(msg) exit(-1)
#define LOG_SOFT_ERROR(msg)
#define LOG_WARNING(msg)
#define LOG_MESSAGE(msg, priority)

#define LOG_ASSERT(condition, msg) if(condition) LOG_ERROR(msg);

#endif
#pragma endregion
