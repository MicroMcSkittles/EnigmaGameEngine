#include "EnigmaSerialization/Utils.h"
#include <Enigma/Core/Core.h>

namespace Enigma {
	namespace Serialization {
		uint16_t SwapEndian16(uint16_t u)
		{
			return
				((u >> 8) & 0x00FF) | // Move byte 2 to byte 1
				((u << 8) & 0xFF00); // Move byte 1 to byte 2
		}
		uint32_t SwapEndian32(uint32_t u)
		{
			return
				((u >> 24) & 0x000000FF) | // Move byte 3 to byte 0
				((u >> 8) & 0x0000FF00) | // Move byte 2 to byte 1
				((u << 8) & 0x00FF0000) | // Move byte 1 to byte 2
				((u << 24) & 0xFF000000); // Move byte 0 to byte 3
		}
		uint64_t SwapEndian64(uint64_t u)
		{
			return 
				((u >> 56) & 0x00000000000000FFULL) |
				((u >> 40) & 0x000000000000FF00ULL) |
				((u >> 24) & 0x0000000000FF0000ULL) |
				((u >> 8)  & 0x00000000FF000000ULL) |
				((u << 8)  & 0x000000FF00000000ULL) |
				((u << 24) & 0x0000FF0000000000ULL) |
				((u << 40) & 0x00FF000000000000ULL) |
				((u << 56) & 0xFF00000000000000ULL);
		}

		uint8_t ReadU8(std::ifstream& file)
		{
			uint8_t value = 0;
			if (!file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
				LOG_SOFT_ERROR("Failed to read uint8 from file");
				return 0;
			}
			return value;
		}
		uint16_t ReadU16(std::ifstream& file, bool littleE)
		{
			uint16_t value = 0;
			if (!file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
				LOG_SOFT_ERROR("Failed to read uint16 from file");
				return 0;
			}
			if (littleE) return value;
			return SwapEndian16(value);
		}
		uint32_t ReadU32(std::ifstream& file, bool littleE)
		{
			uint32_t value = 0;
			if (!file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
				LOG_SOFT_ERROR("Failed to read uint32 from file");
				return 0;
			}
			if (littleE) return value;
			return SwapEndian32(value);
		}
		uint64_t ReadU64(std::ifstream& file, bool littleE)
		{
			uint64_t value = 0;
			if (!file.read(reinterpret_cast<char*>(&value), sizeof(value))) {
				LOG_SOFT_ERROR("Failed to read uint64 from file");
				return 0;
			}
			if (littleE) return value;
			return SwapEndian64(value);
		}

		char ReadS8(std::ifstream& file)
		{
			uint8_t u = ReadU8(file);
			char value = *(char*)&u;
			return value;
		}
		short ReadS16(std::ifstream& file, bool littleE)
		{
			uint16_t u = ReadU16(file, littleE);
			short value = *(short*)&u;
			return value;
		}
		int ReadS32(std::ifstream& file, bool littleE)
		{
			uint32_t u = ReadU32(file, littleE);
			int value = *(int*)&u;
			return value;
		}
		long long ReadS64(std::ifstream& file, bool littleE)
		{
			uint64_t u = ReadU64(file, littleE);
			long long value = *(long long*)&u;
			return value;
		}
	}
}

