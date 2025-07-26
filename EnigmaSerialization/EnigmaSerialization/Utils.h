#pragma once
#include <fstream>

namespace Enigma {
	namespace Serialization {

		// Swaps ints from Big Endian to Little Endian or Little Endian to Big Endian

		uint16_t SwapEndian16(uint16_t u);
		uint32_t SwapEndian32(uint32_t u);
		uint64_t SwapEndian64(uint64_t u);

		// Read in unsigned ints from a binary file

		uint8_t  ReadU8(std::ifstream&  file);
		uint16_t ReadU16(std::ifstream& file, bool littleE = true);
		uint32_t ReadU32(std::ifstream& file, bool littleE = true);
		uint64_t ReadU64(std::ifstream& file, bool littleE = true);

		// Read in signed ints from a binary file

		char      ReadS8(std::ifstream&  file);
		short     ReadS16(std::ifstream& file, bool littleE = true);
		int       ReadS32(std::ifstream& file, bool littleE = true);
		long long ReadS64(std::ifstream& file, bool littleE = true);
	}
}