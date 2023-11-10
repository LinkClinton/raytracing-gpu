#include "stb_image_loader.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

raytracing::runtime::resources::components::cpu_texture raytracing::extensions::textures::load_sdr_texture(
	const std::string& filename)
{
	runtime::resources::components::cpu_texture texture;

	int size_x = 0;
	int size_y = 0;
	int channel = 0;

	stbi_uc* texture_data_sRGB_color_space = stbi_load(filename.c_str(), &size_x, &size_y, &channel, 0);

	// force using fp32/linear space for 8bit/sRGB space texture
	texture.info.size_x = size_x;
	texture.info.size_y = size_y;
	texture.info.channel = channel;
	texture.info.format = runtime::resources::components::texture_format::fp32;

	std::vector<real> texture_data_linear_color_space(size_x * size_y * channel);

	for (size_t index = 0; index < texture_data_linear_color_space.size(); index++)
	{
		texture_data_linear_color_space[index] = sRGB_to_linear(texture_data_sRGB_color_space[index] / 255.0f);
	}

	texture.data.resize(texture_data_linear_color_space.size() * sizeof(real));

	std::memcpy(texture.data.data(), texture_data_linear_color_space.data(), texture.data.size());

	stbi_image_free(texture_data_sRGB_color_space);

	return  texture;
}

raytracing::runtime::resources::components::cpu_texture raytracing::extensions::textures::load_hdr_texture(
	const std::string& filename)
{
	runtime::resources::components::cpu_texture texture;

	int size_x = 0;
	int size_y = 0;
	int channel = 0;

	float* texture_data_linear_color_space = stbi_loadf(filename.c_str(), &size_x, &size_y, &channel, 0);

	texture.info.size_x = size_x;
	texture.info.size_y = size_y;
	texture.info.channel = channel;
	texture.info.format = runtime::resources::components::texture_format::fp32;

	texture.data.resize(size_x * size_y * channel * sizeof(real));

	std::memcpy(texture.data.data(), texture_data_linear_color_space, texture.data.size());

	stbi_image_free(texture_data_linear_color_space);

	return texture;
}
