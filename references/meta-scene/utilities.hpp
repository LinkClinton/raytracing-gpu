#pragma once

#include <string>

namespace metascene {

	using real = float;

	using uint32 = unsigned;

	inline real string_to_real(const std::string& value)
	{
		if constexpr (sizeof(real) == 4)
			return static_cast<real>(std::stof(value));

		if constexpr (sizeof(real) == 8)
			return static_cast<real>(std::stod(value));

		return 0;
	}

	inline bool string_to_bool(const std::string& value)
	{
		if (value == "true" || value == "True") return true;
		return false;
	}

	template <typename T>
	constexpr bool is_integral() {
		return std::is_integral<T>::value && !std::is_unsigned<T>::value;
	}

	template <typename T>
	constexpr bool is_unsigned_integral() {
		return std::is_integral<T>::value && std::is_unsigned<T>::value;
	}

	template <typename T>
	T string_to_integer(const std::string& value)
	{
		if constexpr (is_integral<T>() && sizeof(T) == 4)
			return std::stoi(value);

		if constexpr (is_unsigned_integral<T>() && sizeof(T) == 4)
			return std::stoul(value);
		
		if constexpr (is_unsigned_integral<T>() && sizeof(T) == 8)
			return std::stoull(value);

		throw "unknown error";

		return 0;
	}
}
