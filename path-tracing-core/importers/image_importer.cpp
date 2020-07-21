#include "image_importer.hpp"

#ifdef __IMAGE_IMPORTER__

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

namespace path_tracing::core::importers {

	real inverse_gamma_correct(real value)
	{
		if (value <= 0.04045f) return value * 1.f / 12.92f;
		return pow((value + 0.055f) * 1.f / 1.055f, static_cast<real>(2.4f));
	}

	image_info read_image(const std::string& name, bool gamma)
	{
		auto channel = 0;
		auto width = 0;
		auto height = 0;

		image_info info;

		const auto data = stbi_load(name.c_str(), &width, &height, &channel, STBI_rgb_alpha);

		info.width = width;
		info.height = height;
		info.data = std::vector<real>(info.width * info.height * 4);

		for (size_t y = 0; y < height; y++) {
			const auto invert_y = height - y - 1;

			for (size_t x = 0; x < width; x++) {
				const auto index0 = (y * width + x) * 4;
				const auto index1 = (invert_y * width + x) * 4;

				for (size_t component = 0; component < 4; component++) {
					const auto value = static_cast<real>(data[index1 + component]) / 255;

					info.data[index0 + component] = gamma ? inverse_gamma_correct(value) : value;
				}
			}
		}

		stbi_image_free(data);

		return info;
	}
	
	image_info read_image_hdr(const std::string& name, bool gamma)
	{
		auto channel = 0;
		auto width = 0;
		auto height = 0;

		image_info info;

		const auto data = stbi_loadf(name.c_str(), &width, &height, &channel, STBI_rgb_alpha);

		info.width = width;
		info.height = height;
		info.data = std::vector<real>(info.width * info.height * 4);

		for (size_t y = 0; y < height; y++) {
			const auto invert_y = height - y - 1;

			for (size_t x = 0; x < width; x++) {
				const auto index0 = (y * width + x) * 4;
				const auto index1 = (invert_y * width + x) * 4;

				for (size_t component = 0; component < 4; component++) {
					const auto value = data[index1 + component];

					info.data[index0 + component] = gamma ? inverse_gamma_correct(value) : value;
				}
			}
		}

		stbi_image_free(data);

		return info;
	}

	std::shared_ptr<image> import_environment_map(const std::string& name, bool gamma)
	{
		auto channel = 0;
		auto width = 0;
		auto height = 0;

		image_info info;

		const auto data = stbi_loadf(name.c_str(), &width, &height, &channel, STBI_rgb_alpha);

		info.width = width;
		info.height = height;
		info.data = std::vector<real>(info.width * info.height * 4);

		for (size_t index = 0; index < info.data.size(); index++)
			info.data[index] = gamma ? inverse_gamma_correct(data[index]) : data[index];

		stbi_image_free(data);

		auto values = std::vector<vector3>(info.width * info.height);

		for (size_t index = 0; index < values.size(); index++) {
			values[index].x = info.data[index * 4 + 0];
			values[index].y = info.data[index * 4 + 1];
			values[index].z = info.data[index * 4 + 2];
		}

		return std::make_shared<image>(values, info.width, info.height);
	}

}

#endif