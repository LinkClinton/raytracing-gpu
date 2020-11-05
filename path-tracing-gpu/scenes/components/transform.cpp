#include "transform.hpp"

path_tracing::scenes::transform::transform(const matrix4x4& matrix) : mMatrix(matrix), mInverse(glm::inverse(mMatrix))
{
}

path_tracing::scenes::transform::transform(const matrix4x4& matrix, const matrix4x4& inverse) : mMatrix(matrix), mInverse(inverse)
{
}

path_tracing::scenes::transform path_tracing::scenes::transform::inverse() const noexcept
{
	return transform(mInverse, mMatrix);
}

matrix4x4 path_tracing::scenes::transform::matrix() const noexcept
{
	return mMatrix;
}
