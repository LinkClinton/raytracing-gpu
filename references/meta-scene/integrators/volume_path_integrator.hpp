#pragma once

#include "integrator.hpp"

namespace metascene {

	namespace integrators {

		struct volume_path_integrator final : integrator {
			real threshold = static_cast<real>(1);

			volume_path_integrator();
		};

	}
}
