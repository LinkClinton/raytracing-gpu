#pragma once

#include "resources/shaders/macros.hlsl"

#include "extensions/glm/glm.hpp"

#include <unordered_map>

namespace raytracing
{
	using uint64 = unsigned long long;
	using uint32 = unsigned;
	using uint16 = unsigned short;
	using uint8 = unsigned char;
	using int64 = long long;
	using int32 = int;
	using int16 = short;
	using int8 = char;
	using real = float;
	using byte = unsigned char;

	using vector2 = glm::vec<2, real>;
	using vector3 = glm::vec<3, real>;
	using vector4 = glm::vec<4, real>;

	using matrix4x4 = glm::mat<4, 4, real>;

	using quaternion = glm::qua<real>;
	
	template <typename Key, typename Value>
	using mapping = std::unordered_map<Key, Value>;

	using identity = uint32;

	constexpr uint32 index_null = std::numeric_limits<uint32>::max();

	template <typename T>
	T pi() { return glm::pi<T>(); }

	template <typename T>
	T two_pi() { return glm::two_pi<T>(); }

	template <typename T>
	T sin(T value) { return glm::sin(value); }

	template <typename T>
	T cos(T value) { return glm::cos(value); }

	template <typename T>
	T normalize(const T& value) { return glm::normalize<T>(value); }

	template <typename T>
	T radians(const T& value) { return glm::radians(value); }

	inline matrix4x4 transpose(const matrix4x4& matrix)
	{
		return glm::transpose(matrix);
	}

	inline matrix4x4 perspective_fov(real fov, real size_x, real size_y, real n, real f)
	{
		return glm::perspectiveFovLH(glm::radians(fov), size_x, size_y, n, f);
	}

	inline matrix4x4 translate(const matrix4x4& m, const vector3& v)
	{
		return glm::translate(m, v);
	}

	inline matrix4x4 scale(const matrix4x4& m, const vector3& v)
	{
		return glm::scale(m, v);
	}

	inline matrix4x4 inverse(const matrix4x4& m)
	{
		return glm::inverse(m);
	}

	// enum operators for enum flag
	template <typename Enum>
	concept FlagEnum = requires(Enum e)
	{
		{ mark_enum_as_flag(e) } -> std::convertible_to<void>;
	};

	template <FlagEnum Enum>
	Enum operator|(Enum lhs, Enum rhs)
	{
		return static_cast<Enum>(
			static_cast<std::underlying_type_t<Enum>>(lhs) |
			static_cast<std::underlying_type_t<Enum>>(rhs));
	}

	template <FlagEnum Enum>
	Enum operator|=(Enum& lhs, Enum rhs)
	{
		return lhs = static_cast<Enum>(
			static_cast<std::underlying_type_t<Enum>>(lhs) |
			static_cast<std::underlying_type_t<Enum>>(rhs));
	}

	template <FlagEnum Enum>
	Enum operator&(Enum lhs, Enum rhs)
	{
		return static_cast<Enum>(
			static_cast<std::underlying_type_t<Enum>>(lhs) &
			static_cast<std::underlying_type_t<Enum>>(rhs));
	}

	template <FlagEnum Enum>
	Enum operator&=(Enum& lhs, Enum rhs)
	{
		return lhs = static_cast<Enum>(
			static_cast<std::underlying_type_t<Enum>>(lhs) &
			static_cast<std::underlying_type_t<Enum>>(rhs));
	}
}