#pragma once

#include "integrator.hpp"

namespace metascene {

	namespace integrators {

		struct direct_integrator final : integrator {
			size_t emitter_samples = 4;
			size_t bsdf_samples = 4;

			direct_integrator();
		};
		
	}
}
