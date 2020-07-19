#pragma once

#include "../spectrums/color_spectrum.hpp"
#include "medium.hpp"

namespace metascene {

	using namespace spectrums;

	namespace media {

		struct homogeneous_medium final : medium {
			std::shared_ptr<spectrum> sigma_a;
			std::shared_ptr<spectrum> sigma_s;

			real g = 0;

			homogeneous_medium();

			std::string to_string() const noexcept override;
		};
		
	}
}
