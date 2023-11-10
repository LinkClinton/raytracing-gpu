#include "tiny_exr_loader.hpp"

#include "../spdlog/spdlog.hpp"

#include <tinyexr.h>

raytracing::runtime::resources::components::cpu_texture raytracing::extensions::textures::load_exr_texture(
	const std::string& filename)
{
	runtime::resources::components::cpu_texture texture;

	const char* error_message = nullptr;
	int return_code = 0;

	EXRHeader header;
	EXRVersion version;
	EXRImage image;
	
	return_code = ParseEXRVersionFromFile(&version, filename.c_str());

	if (return_code != 0)
	{
		spdlog::error("parse exr version error.");
		
		return texture;
	}

	return_code = ParseEXRHeaderFromFile(&header, &version, filename.c_str(), &error_message);

	uint32 channel_table[] = 
	{
		std::numeric_limits<uint32>::max(),
		std::numeric_limits<uint32>::max(),
		std::numeric_limits<uint32>::max(),
		std::numeric_limits<uint32>::max(),
	};

	static const mapping<std::string, uint32> channel_to_index = 
	{
		{ "R", 0 },
		{ "G", 1 },
		{ "B", 2 },
		{ "A", 3 }
	};

	for (size_t index = 0; index < header.num_channels; index++)
	{
		if (channel_to_index.contains(header.channels[index].name))
		{
			channel_table[channel_to_index.at(header.channels[index].name)] = static_cast<uint32>(index);
		}

		if (header.pixel_types[index] != 2)
		{
			header.requested_pixel_types[index] = 2;
		}
	}

	std::vector<uint32> channel_order;

	for (size_t index = 0; index < 4; index++)
	{
		if (channel_table[index] != std::numeric_limits<uint32>::max())
		{
			channel_order.push_back(channel_table[index]);
		}
	}

	if (return_code != 0)
	{
		spdlog::error("parse exr header error, message: {0}.", error_message);

		FreeEXRErrorMessage(error_message);

		return texture;
	}
	
	return_code = LoadEXRImageFromFile(&image, &header, filename.c_str(), &error_message);

	if (return_code != 0)
	{
		spdlog::error("load exr image error, message: {0}", error_message);

		FreeEXRErrorMessage(error_message);

		return texture;
	}

	texture.info.size_x = static_cast<uint32>(image.width);
	texture.info.size_y = static_cast<uint32>(image.height);
	texture.info.format = runtime::resources::components::texture_format::fp32;
	texture.info.channel = static_cast<uint32>(channel_order.size());

	std::vector<real> texture_data_linear_space(texture.info.size_x * texture.info.size_y * texture.info.channel);

	for (size_t index = 0; index < channel_order.size(); index++)
	{
		const uint32 target_channel = channel_order[index];

		for (size_t pixel = 0; pixel < texture.info.size_x * texture.info.size_y; pixel++)
		{
			texture_data_linear_space[pixel * texture.info.channel + index] =
				*reinterpret_cast<float*>(image.images[target_channel] + pixel * sizeof(float));
		}
	}

	texture.data.resize(texture.info.size_x * texture.info.size_y * texture.info.channel * sizeof(real));

	std::memcpy(texture.data.data(), texture_data_linear_space.data(), texture.data.size());

	return texture;
}