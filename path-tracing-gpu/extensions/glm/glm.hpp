#pragma once

#include "../../cores/types.hpp"

#include <glm/glm.hpp>

namespace path_tracing::extensions::glm {

	using vector2 = ::glm::vec<2, real>;
	using vector3 = ::glm::vec<3, real>;
	using vector4 = ::glm::vec<4, real>;

	using matrix4x4 = ::glm::mat<4, 4, real>;
	
}

using namespace path_tracing::extensions::glm;