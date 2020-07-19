#pragma once

#include "../utilities.hpp"

namespace metascene {

	namespace samplers {

		enum class type : uint32 {
			unknown = 0, random = 1
		};

		struct sampler {
			size_t sample_per_pixel = 16;

			type type = type::unknown;
			
			sampler() = default;

			sampler(const samplers::type& type);
		};
		
	}
}
