#include "Core/Utilities/Utilities.h"

#include <functional>

namespace Enigma {
	namespace Core {
		const uint64_t Hash(const char* str)
		{
			std::hash<std::string> hasher;
			return hasher(str);
		}
	}
}