#pragma once

#define __IMAGE_IMPORTER__
#ifndef __IMAGE_IMPORTER__
#else

#include "../shared/transform.hpp"
#include "../textures/image.hpp"

#include <filesystem>

namespace path_tracing::core::importers {

	using namespace textures;

	struct image_info {
		std::vector<real> data;

		size_t height;
		size_t width;
	};

	real inverse_gamma_correct(real value);

	image_info read_image(const std::string& name, bool gamma = false);

	image_info read_image_hdr(const std::string& name, bool gamma = false);

	template <typename T>
	std::shared_ptr<image> read_texture2d(const std::string& name, bool gamma = false);

	template <typename T>
	std::shared_ptr<image> read_texture2d_hdr(const std::string& name, bool gamma = false);

	template <typename T>
	std::shared_ptr<image> import_texture2d(const std::string& name, bool gamma = false)
	{
		if (std::filesystem::path(name).extension() == ".hdr")
			return read_texture2d_hdr<T>(name, gamma);

		return read_texture2d<T>(name, gamma);
	}

	std::shared_ptr<image> import_environment_map(const std::string& name, bool gamma = false);

	template <>
	inline std::shared_ptr<image> read_texture2d_hdr<vector3>(const std::string& name, bool gamma)
	{
		const auto image_info = read_image_hdr(name, false);

		auto values = std::vector<vector3>(image_info.width * image_info.height);

		for (size_t index = 0; index < values.size(); index++) {
			values[index].x = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 0]) : image_info.data[index * 4 + 0];
			values[index].y = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 1]) : image_info.data[index * 4 + 1];
			values[index].z = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 2]) : image_info.data[index * 4 + 2];
		}

		return std::make_shared<image>(values, image_info.width, image_info.height);
	}

	template <>
	inline std::shared_ptr<image> read_texture2d<real>(const std::string& name, bool gamma)
	{
		const auto image_info = read_image(name, false);

		auto values = std::vector<real>(image_info.width * image_info.height);

		for (size_t index = 0; index < values.size(); index++)
			values[index] = gamma ? inverse_gamma_correct(image_info.data[index * 4]) : image_info.data[index * 4];

		return std::make_shared<image>(values, image_info.width, image_info.height);
	}

	template <>
	inline std::shared_ptr<image> read_texture2d<vector2>(const std::string& name, bool gamma)
	{
		const auto image_info = read_image(name, false);

		auto values = std::vector<vector2>(image_info.width * image_info.height);

		for (size_t index = 0; index < values.size(); index++) {
			values[index].x = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 0]) : image_info.data[index * 4 + 0];
			values[index].y = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 1]) : image_info.data[index * 4 + 1];
		}

		return std::make_shared<image>(values, image_info.width, image_info.height);
	}

	template <>
	inline std::shared_ptr<image> read_texture2d<vector3>(const std::string& name, bool gamma)
	{
		const auto image_info = read_image(name, false);

		auto values = std::vector<vector3>(image_info.width * image_info.height);

		for (size_t index = 0; index < values.size(); index++) {
			values[index].x = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 0]) : image_info.data[index * 4 + 0];
			values[index].y = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 1]) : image_info.data[index * 4 + 1];
			values[index].z = gamma ? inverse_gamma_correct(image_info.data[index * 4 + 2]) : image_info.data[index * 4 + 2];
		}

		return std::make_shared<image>(values, image_info.width, image_info.height);
	}
}

#endif