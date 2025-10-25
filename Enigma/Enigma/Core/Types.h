#pragma once
#include <memory>

// All of these type defs should make the code easier to read/understand

// Basic types

typedef unsigned char      u8;  // 8 bit unsigned integer
typedef unsigned short     u16; // 16 bit unsigned integer
typedef unsigned int       u32; // 32 bit unsigned integer
typedef unsigned long long u64; // 64 bit unsigned integer

typedef char               i8;  // 8 bit signed integer
typedef short              i16; // 16 bit signed integer
typedef int                i32; // 32 bit signed integer
typedef long long          i64; // 64 bit signed integer

typedef float              f32; // 32 bit floating point number
typedef double             f64; // 64 bit floating point number

namespace Enigma {

	// Smart pointer wrappers

	// Will automatically free memory once all references are destroyed
	template<typename T>
	using ref = std::shared_ptr<T>; 

	template<typename T, typename... Args>
	constexpr ref<T> CreateRef(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T, typename Other>
	inline ref<T> CastRef(Other other) {
		return std::static_pointer_cast<T>(other);
	}

	// Only allows one instance of the pointer to exist, frees memory when destroyed
	template<typename T>
	using unique = std::unique_ptr<T>;

	template<typename T, typename... Args>
	constexpr unique<T> CreateUnique(Args&&... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

}