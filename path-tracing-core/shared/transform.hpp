#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "glm/glm.hpp"

namespace path_tracing::core {

	using uint32 = unsigned int;
	using real = float;

}

namespace path_tracing::core::shared {

	using matrix4x4 = glm::mat<4, 4, real>;
	using vector4 = glm::vec<4, real>;
	using vector3 = glm::vec<3, real>;
	
	struct transform {
		matrix4x4 inverse = matrix4x4(1);
		matrix4x4 matrix = matrix4x4(1);

		transform() = default;

		transform(const matrix4x4& matrix, const matrix4x4& inverse);
		
		transform(const matrix4x4& matrix);

		transform& operator*=(const transform& right);
		transform operator*(const transform& right) const;
	};

	vector3 transform_point(const transform& transform, const vector3& point);

	transform perspective_left_hand(real fov, real width, real height, real near = 0.01, real far = 1000);

	transform translate(const vector3& value);
	
	transform scale(const vector3& value);

	transform inverse(const transform& transform);
}
