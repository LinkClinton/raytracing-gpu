#pragma once

#include "../../types.hpp"

#undef near
#undef far

namespace raytracing::scenes::components
{
	class transform final
	{
	public:
		transform() = default;

		transform(const matrix4x4& matrix);

		transform(const matrix4x4& matrix, const matrix4x4& inverse);

		transform inverse() const noexcept;

		matrix4x4 matrix() const noexcept;
	private:
		matrix4x4 mMatrix = matrix4x4(1), mInverse = matrix4x4(1);
	};

	vector3 transform_point(const transform& transform, const vector3& point);
	
}
