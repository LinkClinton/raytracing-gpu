#pragma once

#include "../math/vector3.hpp"

#include "shape.hpp"

namespace metascene {

	using namespace math;
	
	namespace shapes {

		struct sphere final : shape {
			real radius = static_cast<real>(1);

			sphere();

			std::string to_string() const noexcept override;
		};
		
	}
}
