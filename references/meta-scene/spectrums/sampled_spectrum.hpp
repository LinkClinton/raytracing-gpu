#pragma once

#include "spectrum.hpp"

#include <vector>

namespace metascene {

	namespace spectrums {

		struct sampled_spectrum final : spectrum {
			std::vector<real> lambda;
			std::vector<real> value;

			sampled_spectrum();

			std::string to_string() const noexcept override;
		};

		std::shared_ptr<sampled_spectrum> read_sampled_spectrum_from_spd(const std::string& filename);

		std::shared_ptr<sampled_spectrum> create_sampled_spectrum_from_black_body(const std::vector<real>& lambda, real temperature, real scale);

		std::shared_ptr<sampled_spectrum> create_sampled_spectrum_from_black_body(real temperature, real scale);

		std::shared_ptr<sampled_spectrum> black_body(const std::vector<real>& lambda, real temperature);
	}
}
