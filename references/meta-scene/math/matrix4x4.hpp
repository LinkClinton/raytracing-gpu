#pragma once

#include "vector3.hpp"

namespace metascene {

	namespace math {

#ifdef __GLM_IMPLEMENTATION__

		template <typename T>
		using matrix4x4_t = glm::mat<4, 4, T>;

		using matrix4x4 = matrix4x4_t<real>;

		template <typename T>
		matrix4x4_t<T> look_at_right_hand(const vector3& origin, const vector3& target, const vector3& up)
		{
			return glm::lookAtRH(origin, target, up);
		}

		template <typename T>
		matrix4x4_t<T> look_at_left_hand(const vector3& origin, const vector3& target, const vector3& up)
		{
			return glm::lookAtLH(origin, target, up);
		}

		template <typename T>
		matrix4x4_t<T> inverse(const matrix4x4_t<T>& matrix)
		{
			return glm::inverse(matrix);
		}

		template <typename T>
		matrix4x4_t<T> translate(const vector3& v)
		{
			return glm::translate(matrix4x4_t<T>(1), v);
		}

		template <typename T>
		matrix4x4_t<T> rotate(const vector3& axis, real angle)
		{
			return glm::rotate(matrix4x4_t<T>(1), angle, axis);
		}

		template <typename T>
		matrix4x4_t<T> scale(const vector3& v)
		{
			return glm::scale(matrix4x4_t<T>(1), v);
		}

		template <typename T>
		T radians(T value)
		{
			return glm::radians(value);
		}
#endif
		
	}
}
