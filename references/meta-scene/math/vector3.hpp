#pragma once

#define __GLM_IMPLEMENTATION__
#ifdef  __GLM_IMPLEMENTATION__
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#endif

#include "../utilities.hpp"

#include <sstream>

namespace metascene {

	namespace math {

#ifdef __GLM_IMPLEMENTATION__

		template <typename T>
		using vector3_t = glm::vec<3, T>;

		using vector3 = vector3_t<real>;

		inline std::string to_string(const vector3& v)
		{
			std::stringstream stream;

			stream << "[x = " << v.x << ", y = " << v.y << ", z = " << v.z << "]";

			return stream.str();
		}
#endif
		
	}
	
}
