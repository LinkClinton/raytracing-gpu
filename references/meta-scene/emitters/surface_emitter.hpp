#pragma once

#include "../spectrums/color_spectrum.hpp"
#include "../spectrums/spectrum.hpp"
#include "emitter.hpp"

#include <memory>

namespace metascene {

	using namespace spectrums;
	
	namespace emitters {

		struct surface_emitter final : emitter {
			std::shared_ptr<spectrum> radiance = std::make_shared<color_spectrum>();

			surface_emitter();

			std::string to_string() const noexcept override;
		};
		
	}
}
