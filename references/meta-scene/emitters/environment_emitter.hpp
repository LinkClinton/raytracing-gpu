#pragma once

#include "../spectrums/color_spectrum.hpp"
#include "../spectrums/spectrum.hpp"
#include "emitter.hpp"

namespace metascene {

	using namespace spectrums;
	
	namespace emitters {

		struct environment_emitter final : emitter {
			std::shared_ptr<spectrum> intensity = std::make_shared<color_spectrum>();
			
			std::string environment_map = "";

			bool gamma = false;

			environment_emitter();

			std::string to_string() const noexcept override;
		};
		
	}
}
