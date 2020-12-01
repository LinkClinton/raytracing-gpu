#pragma once

#include "components/perspective_camera.hpp"
#include "components/transform.hpp"

#include "entity.hpp"

#include <optional>

namespace path_tracing::scenes {

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

		uint64 sample_count = std::numeric_limits<uint64>::max();
		uint32 max_depth = 5;
		
		perspective_camera camera;
		
		std::vector<entity> entities;

		std::optional<output_window_property> output_window;
		std::optional<output_images_property> output_images;
	};

	std::tuple<matrix4x4, matrix4x4> compute_camera_matrix(const scene& scene);

}
