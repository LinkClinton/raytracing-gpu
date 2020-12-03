#pragma once

#include "../../types.hpp"

namespace path_tracing::scenes::components {

	struct film final {
		uint32 size_x = 0;
		uint32 size_y = 0;
		real scale = 1;

		film() = default;
	};
	
}
