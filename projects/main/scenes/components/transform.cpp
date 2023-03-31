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
