#pragma once

#include "extensions/glm/glm.hpp"

#include <unordered_map>

namespace path_tracing {

	using uint64 = unsigned long long;
	using uint32 = unsigned;
	using real = float;
	using byte = unsigned char;

	using vector2 = glm::vec<2, real>;
	using vector3 = glm::vec<3, real>;
	using vector4 = glm::vec<4, real>;

	using matrix4x4 = glm::mat<4, 4, real>;

	template <typename Key, typename Value>
	using mapping = std::unordered_map<Key, Value>;

	using identity = uint32;
	
	constexpr uint32 index_null = std::numeric_limits<uint32>::max();

	enum class coordinate_system : uint32 {
		right_hand = 0, left_hand = 1
	};
	
}