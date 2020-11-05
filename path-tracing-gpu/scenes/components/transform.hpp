#pragma once

#include "../../extensions/glm/glm.hpp"

namespace path_tracing::scenes {

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
	
}
