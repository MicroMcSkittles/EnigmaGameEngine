#include "Enigma/EnigmaPCH.h"
#include "Enigma/Core/Utilities/Utilities.h"
#include "Enigma/Core/Core.h"

namespace Enigma::Core{
	const u64 Hash(const std::string& str)
	{
		std::hash<std::string> hasher;
		return hasher(str);
	}
	const u64 Hash(const std::string& str1, const std::string& str2)
	{
		std::hash<std::string> hasher;
		return hasher(str1 + str2);
	}
	const uint64_t Hash(const char* str)
	{
		std::hash<std::string> hasher;
		return hasher(str);
	}
	const uint64_t Hash(const char* str1, const char* str2)
	{
		return Hash(std::string(str1), std::string(str2));
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