#pragma once

#include "integrator.hpp"

namespace metascene {

	namespace integrators {

		struct photon_mapping_integrator final : integrator {
			size_t iterations = 64;
			size_t photons = 0;
			
			real radius = 1;

			photon_mapping_integrator();
		};
		
	}
}
