#pragma once

#include "components/perspective_camera.hpp"
#include "components/transform.hpp"

#include "entity.hpp"

#include <optional>

namespace path_tracing::scenes {

	struct output_window_property {
		std::string name = "";

		uint32 font_size = 20;
		
		output_window_property() = default;
	};
	
	struct scene final {
		coordinate_system texture_system = coordinate_system::left_hand;
		coordinate_system camera_system = coordinate_system::right_hand;

		perspective_camera camera;
		
		std::vector<entity> entities;

		std::optional<output_window_property> output_window;
	};
	
}
