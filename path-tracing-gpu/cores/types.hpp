#pragma once

#include "../../extensions/glm/glm.hpp"

#include <unordered_map>

namespace path_tracing::cores {
	
	using uint32 = unsigned;
	using real = float;

	using vector2 = glm::vec<2, real>;
	using vector3 = glm::vec<3, real>;
	using vector4 = glm::vec<4, real>;

	using matrix4x4 = glm::mat<4, 4, real>;

	template <typename Key, typename Value>
	using mapping = std::unordered_map<Key, Value>;
}

using namespace path_tracing::cores;