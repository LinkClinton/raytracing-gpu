#pragma once

#include "../spectrums/color_spectrum.hpp"
#include "../math/vector3.hpp"
#include "emitter.hpp"

namespace metascene {

	using namespace spectrums;
	using namespace math;
	
	namespace emitters {

		struct directional_emitter final : emitter {
			std::shared_ptr<spectrum> irradiance = std::make_shared<color_spectrum>();

			vector3 from = vector3(0, 0, 1);
			vector3 to = vector3(0);

			directional_emitter();

			std::string to_string() const noexcept override;
		};
		
	}
}
