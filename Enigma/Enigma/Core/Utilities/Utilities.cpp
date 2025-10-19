#include "Enigma/Core/Utilities/Utilities.h"
#include "Enigma/Core/Core.h"

#include <functional>
#include <cstring>

namespace Enigma {
	namespace Core {
		const uint64_t Hash(const char* str)
		{
			std::hash<std::string> hasher;
			return hasher(str);
		}
		const uint64_t Hash(const char* str1, const char* str2)
		{
			// Plus 1 for null terminator
			size_t length = strlen(str1) + strlen(str2) + 1;
			char* concatStr = static_cast<char*>(malloc(length));

			// Make sure the memory allocation was successful
			if (!concatStr) {
				LOG_ERROR("Failed to allocat memory for string hash ( %s, %s )", str1, str2);
				return (uint64_t)-1;
			}

			// Combine both strings
			strcpy(concatStr, str1);
			strcat(concatStr, str2);

			// Find hash
			uint64_t strHash = Hash(concatStr);

			// Free allocated memory
			free(concatStr);

			return strHash;
		}
	}
}