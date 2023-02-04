#pragma once

#include "../types.hpp"

#include "components/perspective_camera.hpp"
#include "components/film.hpp"

#include <optional>

namespace raytracing::scenes {

	using namespace components;

	struct output_window_property {
		std::string name = "";

		uint32 font_size = 20;
		uint32 size_x = 1920;
		uint32 size_y = 1080;
		
		bool enable = true;
		
		output_window_property() = default;
	};

	struct output_images_property {
		std::string sdr_image = "";
		std::string hdr_image = "";

		output_images_property() = default;
	};
	
	struct scene final {
		coordinate_system texture_system = coordinate_system::left_hand;
		coordinate_system camera_system = coordinate_system::right_hand;

		uint64 sample_count = 64;
		uint32 max_depth = 5;

		perspective_camera camera;
		film film;

		std::optional<output_window_property> output_window;
		std::optional<output_images_property> output_images;

	};

}
