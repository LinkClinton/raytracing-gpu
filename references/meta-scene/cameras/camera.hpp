#pragma once

#include "../math/matrix4x4.hpp"
#include "../utilities.hpp"

namespace metascene {

	using namespace math;
	
	namespace cameras {

		enum class type : uint32 {
			unknown = 0, perspective = 1
		};

		struct camera {
			matrix4x4 transform = matrix4x4(1);

			type type = type::unknown;

			camera() = default;

			camera(const cameras::type& type);
		};
	}	
}
