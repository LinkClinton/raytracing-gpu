#include "transform.hpp"

path_tracing::core::shared::transform::transform(const matrix4x4& matrix) :
	inverse(glm::inverse(matrix)), matrix(matrix)
{	
}

path_tracing::core::shared::vector3 path_tracing::core::shared::transform_point(const transform& transform, const vector3& point)
{
	return transform.matrix * vector4(point, 1);
}
