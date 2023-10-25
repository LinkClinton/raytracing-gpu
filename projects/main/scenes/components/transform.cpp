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

raytracing::vector3 raytracing::scenes::components::transform_point(const transform& transform, const vector3& point)
{
	return transform.matrix() * vector4(point, 1);
}

raytracing::matrix4x4 raytracing::scenes::components::compute_raster_to_camera(
	real fov, real size_x, real size_y, real near, real far)
{
	const matrix4x4 camera_to_screen = perspective_fov(radians(fov), size_x, size_y, near, far);

	matrix4x4 screen_to_raster = scale(matrix4x4(1), vector3(size_x, size_y, 1));

	screen_to_raster = scale(screen_to_raster, vector3(0.5f, 0.5f, 1));
	screen_to_raster = translate(screen_to_raster, vector3(1.0f, 1.0f, 0));

	return transpose(inverse(camera_to_screen) * inverse(screen_to_raster));
}

