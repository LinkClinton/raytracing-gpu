#pragma once

#include "../../cores/types.hpp"
#include "transform.hpp"

namespace path_tracing::scenes::components {

	struct perspective_camera final {
		transform transform = components::transform();
		
		vector2 resolution = vector2(0);

		real fov = 45.f;

		perspective_camera() = default;
	};
	
}
