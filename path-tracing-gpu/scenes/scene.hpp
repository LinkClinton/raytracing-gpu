#pragma once

#include "components/perspective_camera.hpp"
#include "components/transform.hpp"

#include "entity.hpp"

namespace path_tracing::scenes {

	struct scene final {
		coordinate_system texture_system = coordinate_system::left_hand;
		coordinate_system camera_system = coordinate_system::right_hand;

		perspective_camera camera;
		
		std::vector<entity> entities;

		std::string output_window = "";
	};
	
}
