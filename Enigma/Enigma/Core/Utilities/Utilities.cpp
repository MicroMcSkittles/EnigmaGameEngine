#include "Enigma/EnigmaPCH.h"
#include "Enigma/Core/Utilities/Utilities.h"
#include "Enigma/Core/Core.h"

namespace Enigma::Core{
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

	std::string Format::PostProcessFormated(const std::string& formated)
	{
		std::string result;
		u64 start = 0;
		u64 end = 0;
		while ((start = formated.find("{{", end)) != std::string::npos) {
			result.append(formated.substr(end, start));
			end = start + 2;
		}
		result.append(formated.substr(end, formated.size()));
		return result;
	}

	std::vector<std::pair<u64, u64>> Format::GetFormatPositionsFromString(const std::string& unformated)
	{
		std::vector<std::pair<u64, u64>> result;

		u64 start = 0;
		u64 end = 0;
		// Loop through each open bracket in the string
		while ((start = unformated.find_first_of("{", end)) != std::string::npos) {
			// if a forward slash is before the bracket than skip the bracket
			if (unformated[std::max(0ull, start - 1ull)] == '{') {
				start += 1;
				continue;
			}

			// Find the closing bracket
			end = unformated.find_first_of("}", start);

			// If no closing bracket is found than throw exeption
			if (end == std::string::npos) throw std::runtime_error("Expected closing bracket in formated string.");

			// Store position
			result.push_back(std::pair<u64, u64>(start, end));
		}
		return result;
	}
	std::string Format::GetFormatArguments(const std::string& unformated, const std::pair<u64, u64>& position)
	{
		return unformated.substr(position.first + 1, position.second - position.first - 1);
	}

	u64 Format::GetElementIndex(const std::string& arguments, u64 formatIndex)
	{
		if (arguments.empty()) return formatIndex;
		if (arguments.find_first_not_of("1234567890") != std::string::npos) throw std::runtime_error("Invalid string format arguments");
		return std::stoull(arguments);
	}
	
}