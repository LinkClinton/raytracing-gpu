#pragma once

#include "../../types.hpp"

#undef near
#undef far

namespace raytracing::scenes::components {

	class transform final {
	public:
		transform() = default;

		transform(const matrix4x4& matrix);

		transform(const matrix4x4& matrix, const matrix4x4& inverse);

		~transform() = default;

		transform inverse() const noexcept;

		matrix4x4 matrix() const noexcept;
	private:
		matrix4x4 mMatrix = matrix4x4(1), mInverse = matrix4x4(1);
	};

	matrix4x4 perspective_fov(const coordinate_system& system, real fov, real width, real height, real near, real far);

	matrix4x4 look_at(const coordinate_system& system, const vector3& eye, const vector3& at, const vector3& up);

	vector3 transform_point(const transform& transform, const vector3& point);
	
}
