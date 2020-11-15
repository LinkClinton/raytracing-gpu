#include "transform.hpp"

path_tracing::scenes::components::transform::transform(const matrix4x4& matrix) : mMatrix(matrix), mInverse(glm::inverse(matrix))
{
}

path_tracing::scenes::components::transform::transform(const matrix4x4& matrix, const matrix4x4& inverse) : mMatrix(matrix), mInverse(inverse)
{
}

path_tracing::scenes::components::transform path_tracing::scenes::components::transform::inverse() const noexcept
{
	return transform(mInverse, mMatrix);
}

path_tracing::matrix4x4 path_tracing::scenes::components::transform::matrix() const noexcept
{
	return mMatrix;
}

path_tracing::matrix4x4 path_tracing::scenes::components::perspective_fov(const coordinate_system& system, real fov, real width,
	real height, real near, real far)
{
	if (system == coordinate_system::right_hand)
		return glm::perspectiveFovRH(fov, width, height, near, far);
	else
		return glm::perspectiveFovLH(fov, width, height, near, far);
}

path_tracing::matrix4x4 path_tracing::scenes::components::look_at(const coordinate_system& system, const vector3& eye,
	const vector3& at, const vector3& up)
{
	if (system == coordinate_system::right_hand)
		return glm::lookAtRH(eye, at, up);
	else
		return glm::lookAtLH(eye, at, up);
}
