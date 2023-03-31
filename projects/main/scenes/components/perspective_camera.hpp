#pragma once

#include "../../types.hpp"
#include "transform.hpp"

namespace raytracing::scenes::components
{
	struct perspective_camera final
	{
		transform transform = components::transform();
		
		real fov = 45.f;

		perspective_camera() = default;
	};
}
