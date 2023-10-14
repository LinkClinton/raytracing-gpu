#pragma once

#include "../types.hpp"

#include "components/perspective_camera.hpp"
#include "components/film.hpp"
#include "entity.hpp"

#include <optional>
#include <string>

namespace raytracing::scenes
{
	using namespace components;

	struct output_window_property final
	{
		std::string name = "";

		uint32 font_size = 20;
		uint32 size_x = 1920;
		uint32 size_y = 1080;

		bool enable = true;
	};

	struct output_images_property final
	{
		std::string sdr_image = "";
		std::string hdr_image = "";
	};

	struct scene final
	{
		uint64 sample_count = 64;
		uint32 max_depth = 5;

		perspective_camera camera;
		film film;

		std::vector<entity> entities;

		std::optional<output_window_property> output_window;
		std::optional<output_images_property> output_images;
	};

}