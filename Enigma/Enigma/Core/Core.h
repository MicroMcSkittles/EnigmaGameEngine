#pragma once
#include <cstdlib>

#pragma region utils

#define BIT(x) 1 << x

#pragma endregion

#pragma region Logger Impl
#ifdef DEBUG
#define ENABLE_LOGGER
#endif

#ifdef ENABLE_LOGGER

#include "Core/Logger.h"

#define INIT_LOGGER(config) Enigma::Core::Logger::Init(config)

#define LOG_ERROR(msg) Enigma::Core::Logger::Log(msg, 0, Enigma::Core::LogType::Error, { __FILE__, __func__, __LINE__  })
#define LOG_SOFT_ERROR(msg) Enigma::Core::Logger::Log(msg, 0, Enigma::Core::LogType::SoftError, { __FILE__, __func__, __LINE__  })
#define LOG_WARNING(msg) Enigma::Core::Logger::Log(msg, 1, Enigma::Core::LogType::Warning, { __FILE__, __func__, __LINE__  })
#define LOG_MESSAGE(msg, priority) Enigma::Core::Logger::Log(msg, priority, Enigma::Core::LogType::Message, { __FILE__, __func__, __LINE__  })

#else

#define INIT_LOGGER(config)

#define LOG_ERROR(msg) exit(-1)
#define LOG_SOFT_ERROR(msg)
#define LOG_WARNING(msg)
#define LOG_MESSAGE(msg, priority)

#endif
#pragma endregion
