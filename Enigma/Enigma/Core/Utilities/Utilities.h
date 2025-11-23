#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <sstream>

#include "Enigma/Core/Types.h"

namespace Enigma {
	class Formatter {
	public:
		static std::string FormatValue(const std::string& value) { return value; }
		static std::string FormatValue(const char* value) { return value; }
		static std::string FormatValue(i8 value) { return std::to_string(value); }
		static std::string FormatValue(i16 value) { return std::to_string(value); }
		static std::string FormatValue(i32 value) { return std::to_string(value); }
		static std::string FormatValue(i64 value) { return std::to_string(value); }
		static std::string FormatValue(u8 value) { return std::to_string(value); }
		static std::string FormatValue(u16 value) { return std::to_string(value); }
		static std::string FormatValue(u32 value) { return std::to_string(value); }
		static std::string FormatValue(u64 value) { return std::to_string(value); }
		static std::string FormatValue(f32 value) { return std::to_string(value); }
		static std::string FormatValue(f64 value) { return std::to_string(value); }
	};
}

namespace Enigma::Core {
	const u64 Hash(const char* str);
	const u64 Hash(const char* str1, const char* str2);

	// Contains functions used to format strings
	namespace Format {
		// Replace double brackets with single brackets
		std::string PostProcessFormated(const std::string& formated);
		// returns a list of start and end points of format values in the string
		std::vector<std::pair<u64, u64>> GetFormatPositionsFromString(const std::string& unformated);
		// returns the arguments inside of the format brackets and the strings to the left and right of the arguments
		std::string GetFormatArguments(const std::string& unformated, const std::pair<u64, u64>& position);
		u64 GetElementIndex(const std::string& arguments, u64 formatIndex);
	
		template<typename... ElementTypes>
		class ElementGetter {
		public:

			template <typename Tuple, u64... Indices>
			static auto MakeElementGettersImpl(Tuple& t, std::index_sequence<Indices...>) {
				return std::vector<std::function<std::string()>> {
					[&t]() { return Formatter::FormatValue(std::get<Indices>(t)); }...
				};
			}

			template <typename Tuple>
			static auto MakeElementGetters(Tuple& t) {
				return MakeElementGettersImpl(t, std::make_index_sequence<std::tuple_size<Tuple>::value>{});
			}
		};
	}

	// Do not pass const char* durrectly
	template <typename... Elements>
	std::string FormatString(const std::string& unformated, Elements&&... elements) {
		
		std::string formatted;

		std::tuple<Elements...> elementTuple(std::forward<Elements>(elements)...);
		auto getters = Format::ElementGetter<Elements...>::MakeElementGetters(elementTuple);

		std::vector<std::pair<u64, u64>> positions = Format::GetFormatPositionsFromString(unformated);
		for (u64 i = 0; i < positions.size(); i++) {
			std::pair<u64, u64>& position = positions[i];

			std::string argument = Format::GetFormatArguments(unformated, position);
			u64 elementIndex = std::min(Format::GetElementIndex(argument, i), sizeof...(Elements) - 1);

			u64 lastEnd = ((i == 0) ? 0 : positions[i - 1].second + 1);
			formatted.append(unformated.substr(lastEnd, position.first - lastEnd));
			formatted.append(getters[elementIndex]());
		}
		formatted.append(unformated.substr(positions[positions.size() - 1].second + 1, unformated.size()));

		Format::PostProcessFormated(formatted);

		return formatted;
	}
}