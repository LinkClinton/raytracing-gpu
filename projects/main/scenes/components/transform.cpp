#include "transform.hpp"

raytracing::scenes::components::transform::transform(const matrix4x4& matrix) : mMatrix(matrix), mInverse(glm::inverse(matrix))
{
}

raytracing::scenes::components::transform::transform(const matrix4x4& matrix, const matrix4x4& inverse) : mMatrix(matrix), mInverse(inverse)
{
}

raytracing::scenes::components::transform raytracing::scenes::components::transform::inverse() const noexcept
{
	return transform(mInverse, mMatrix);
}

raytracing::matrix4x4 raytracing::scenes::components::transform::matrix() const noexcept
{
	return mMatrix;
}

raytracing::matrix4x4 raytracing::scenes::components::perspective_fov(const coordinate_system& system, real fov, real width,
	real height, real near, real far)
{
	if (system == coordinate_system::right_hand)
		return glm::perspectiveFovRH(fov, width, height, near, far);
	else
		return glm::perspectiveFovLH(fov, width, height, near, far);
}

raytracing::matrix4x4 raytracing::scenes::components::look_at(const coordinate_system& system, const vector3& eye,
	const vector3& at, const vector3& up)
{
	if (system == coordinate_system::right_hand)
		return glm::lookAtRH(eye, at, up);
	else
		return glm::lookAtLH(eye, at, up);
}

raytracing::vector3 raytracing::scenes::components::transform_point(const transform& transform, const vector3& point)
{
	return transform.matrix() * vector4(point, 1);
}
