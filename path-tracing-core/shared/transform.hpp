#pragma once

#include "glm/glm.hpp"

namespace path_tracing::core::shared {

	using uint32 = unsigned int;
	using real = float;

	using matrix4x4 = glm::mat4x4<real>;
	using vector4 = glm::vec4<real>;
	using vector3 = glm::vec3<real>;
	
	struct transform {
		matrix4x4 inverse = matrix4x4(1);
		matrix4x4 matrix = matrix4x4(1);

		transform() = default;

		transform(const matrix4x4& matrix);
	};

	vector3 transform_point(const transform& transform, const vector3& point);
}
