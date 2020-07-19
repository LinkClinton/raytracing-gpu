#pragma once

#include "../math/vector3.hpp"
#include "shape.hpp"

#include <array>

namespace metascene {

	using namespace math;
	
	namespace shapes {

		struct curve : shape {
			std::array<vector3, 4> control_points = {};
			std::array<real, 2> width = { 1, 1 };

			real u_min = 0, u_max = 1;

			curve();

			std::string to_string() const noexcept override;
		};
		
	}
}
