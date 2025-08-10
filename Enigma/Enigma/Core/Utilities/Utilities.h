#pragma once
#include <string>
#include <stdint.h>

namespace Enigma {
	namespace Core {
		const uint64_t Hash(const char* str);
		const uint64_t Hash(const char* str1, const char* str2);
	}
}