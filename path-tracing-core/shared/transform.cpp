#include "transform.hpp"

path_tracing::core::shared::transform::transform(const matrix4x4& matrix, const matrix4x4& inverse) :
	inverse(inverse), matrix(matrix)
{
}

path_tracing::core::shared::transform::transform(const matrix4x4& matrix) :
	inverse(glm::inverse(matrix)), matrix(matrix)
{	
}

path_tracing::core::shared::transform& path_tracing::core::shared::transform::operator*=(const transform& right)
{
	matrix = matrix * right.matrix;
	inverse = right.inverse * inverse;

	return *this;
}

path_tracing::core::shared::transform path_tracing::core::shared::transform::operator*(const transform& right) const
{
	return transform(
		matrix * right.matrix,
		right.inverse * inverse);
}

path_tracing::core::shared::vector3 path_tracing::core::shared::transform_point(const transform& transform, const vector3& point)
{
	return transform.matrix * vector4(point, 1);
}

path_tracing::core::shared::transform path_tracing::core::shared::perspective_left_hand(real fov, real width, real height, real near, real far)
{
	const auto matrix = glm::perspectiveFovLH(fov, width, height, near, far);

	return transform(matrix);
}

path_tracing::core::shared::transform path_tracing::core::shared::translate(const vector3& value)
{
	return transform(glm::translate(matrix4x4(1), value), glm::translate(matrix4x4(1), -value));
}

path_tracing::core::shared::transform path_tracing::core::shared::scale(const vector3& value)
{
	return transform(glm::scale(matrix4x4(1), value), glm::scale(matrix4x4(1), static_cast<real>(1) / value));
}

path_tracing::core::shared::transform path_tracing::core::shared::inverse(const transform& transform)
{
	return shared::transform(transform.inverse, transform.matrix);
}
