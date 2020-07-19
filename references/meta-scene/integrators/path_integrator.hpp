#pragma once

#include "integrator.hpp"

namespace metascene {

	namespace integrators {

		struct path_integrator final : integrator {
			real threshold = static_cast<real>(1);
			
			path_integrator();
		};
		
	}
}
