#pragma once

#include "../utilities.hpp"

#include <string>

namespace metascene {

	namespace integrators {

		enum class type : unsigned {
			unknown = 0, path = 1, direct = 2, volume_path = 3, photon_mapping = 4, bidirectional_path = 5
		};
		
		struct integrator {
			type type = type::unknown;

			size_t depth = 5;

			integrator() = default;

			integrator(const integrators::type& type);
		};
	
	}
}
