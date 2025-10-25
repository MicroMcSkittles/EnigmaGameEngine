#pragma once
#include <string>
#include <stdint.h>

#include "Enigma/Core/Types.h"

namespace Enigma {
	namespace Core {
		const u64 Hash(const char* str);
		const u64 Hash(const char* str1, const char* str2);
	}
}