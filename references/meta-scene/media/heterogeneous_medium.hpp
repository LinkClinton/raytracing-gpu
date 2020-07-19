#pragma once

#include "../spectrums/color_spectrum.hpp"
#include "../math/matrix4x4.hpp"
#include "medium.hpp"

#include <vector>

namespace metascene {

	using namespace spectrums;
	using namespace math;
	
	namespace media {

		struct heterogeneous_medium final : medium {
			std::shared_ptr<spectrum> albedo;
			
			std::vector<real> sigma_t;

			matrix4x4 transform = matrix4x4(1);
			
			size_t x = 1, y = 1, z = 1;

			real g = 0;

			heterogeneous_medium();

			std::string to_string() const noexcept override;
		};
		
	}
}
