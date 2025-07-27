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

#pragma region Debug Break

#ifdef RELEASE
#    define DEBUG_BREAK
#elif DEBUG
#    ifdef PLATFORM_WINDOWS
#        define DEBUG_BREAK __debugbreak();
#    elif PLATFORM_LINUX
#        define DEBUG_BREAK asm("int3");
#    endif
#endif 

#pragma endregion

#pragma region Logger Impl
#ifdef DEBUG
#    define ENABLE_LOGGER
#endif
#define ENABLE_LOGGER

#ifdef ENABLE_LOGGER

#    include "Enigma/Core/Utilities/Logger.h"

#    define INIT_LOGGER(config)        Enigma::Core::Logger::Init(config)
#    define SET_LOGGER_FLAGS(flags)    Enigma::Core::Logger::GetConfig()->flags = flags;
#    define SET_LOG_CALLBACK(callback) Enigma::Core::Logger::GetConfig()->logCallback = callback;

#    ifdef PLATFORM_WINDOWS
#        define LOG_ERROR(msg, ...) { DEBUG_BREAK Enigma::Core::Logger::Log(msg, { Enigma::Core::LogType::Error,     0,        __FILE__, __func__, __LINE__ },   __VA_ARGS__); }
#        define LOG_SOFT_ERROR(msg, ...)          Enigma::Core::Logger::Log(msg, { Enigma::Core::LogType::SoftError, 0,        __FILE__, __func__, __LINE__ },   __VA_ARGS__);
#        define LOG_WARNING(msg, ...)             Enigma::Core::Logger::Log(msg, { Enigma::Core::LogType::Warning,   1,        __FILE__, __func__, __LINE__ },   __VA_ARGS__)
#        define LOG_MESSAGE(msg, priority, ...)   Enigma::Core::Logger::Log(msg, { Enigma::Core::LogType::Message,   priority, __FILE__, __func__, __LINE__ },   __VA_ARGS__)
#        define LOG_ASSERT(condition, msg, ...)   if(condition) LOG_ERROR(msg, __VA_ARGS__);
#    elif PLATFORM_LINUX
#        define LOG_ERROR(msg, ...) { DEBUG_BREAK Enigma::Core::Logger::Log(msg, { Enigma::Core::LogType::Error,     0,        __FILE__, __func__, __LINE__ }, ##__VA_ARGS__); }
#        define LOG_SOFT_ERROR(msg, ...)          Enigma::Core::Logger::Log(msg, { Enigma::Core::LogType::SoftError, 0,        __FILE__, __func__, __LINE__ }, ##__VA_ARGS__)
#        define LOG_WARNING(msg, ...)             Enigma::Core::Logger::Log(msg, { Enigma::Core::LogType::Warning,   1,        __FILE__, __func__, __LINE__ }, ##__VA_ARGS__)
#        define LOG_MESSAGE(msg, priority, ...)   Enigma::Core::Logger::Log(msg, { Enigma::Core::LogType::Message,   priority, __FILE__, __func__, __LINE__ }, ##__VA_ARGS__)
#        define LOG_ASSERT(condition, msg, ...)   if(condition) LOG_ERROR(msg, ##__VA_ARGS__);
#    endif
#else
#    define INIT_LOGGER(config)
#    define SET_LOGGER_FLAGS(flags)
#    define SET_LOG_CALLBACK(callback)

#    define LOG_ERROR(msg, ...) exit(-1)
#    define LOG_SOFT_ERROR(msg, ...)
#    define LOG_WARNING(msg, ...)
#    define LOG_MESSAGE(msg, priority, ...)
     
#    define LOG_ASSERT(condition, msg, ...) if(condition) LOG_ERROR(msg);
#endif
#pragma endregion
