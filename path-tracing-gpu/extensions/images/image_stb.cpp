#include "image_stb.hpp"

#include "../../runtime/render/render_device.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <stb_image_write.h>
#include <stb_image.h>

namespace path_tracing::extensions::images {

	real inverse_gamma_correct(real value)
	{
		if (value <= 0.04045f) return value * 1.f / 12.92f;
		return pow((value + 0.055f) * 1.f / 1.055f, static_cast<real>(2.4f));
	}
	
}

void path_tracing::extensions::images::write_image_to_png(const std::string& filename, const image_info<byte>& image)
{
	stbi_write_png(filename.c_str(), static_cast<int>(image.size_x),
		static_cast<int>(image.size_y), 4, image.values.data(), 0);
}

void path_tracing::extensions::images::write_image_to_png(const std::string& filename,
	const wrapper::directx12::texture2d& texture2d, const runtime::render::render_device& device)
{
	const auto command_allocator = wrapper::directx12::command_allocator::create(device.device());
	const auto command_list = wrapper::directx12::graphics_command_list::create(device.device(), command_allocator);

	command_allocator->Reset();
	command_list->Reset(command_allocator.get(), nullptr);

	const auto render_target_read_buffer = wrapper::directx12::buffer::create(
		device.device(),
		wrapper::directx12::resource_info::readback(),
		texture2d.alignment() * texture2d.size_y()
	);

	render_target_read_buffer.copy_from(command_list, texture2d);

	command_list.close();

	device.queue().execute({ command_list });
	device.wait();

	// see https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
	// DXGI_FORMAT_XXX_UNORM: Unsigned normalized integer; which is interpreted in a resource as an unsigned integer
	// and is interpreted in a shader as an unsigned normalized floating-point value in the range [0, 1].
	auto cpu_image_data = std::vector<byte>(texture2d.size_x() * texture2d.size_y() * 4);
	auto gpu_image_data = static_cast<byte*>(render_target_read_buffer.begin_mapping());

	for (size_t y = 0; y < texture2d.size_y(); y++) {
		std::memcpy(cpu_image_data.data() + y * texture2d.size_x() * 4,
			gpu_image_data + y * texture2d.alignment(),
			texture2d.size_x() * 4);
	}

	render_target_read_buffer.end_mapping();

	write_image_to_png(filename, { cpu_image_data, texture2d.size_x(), texture2d.size_y() });
}

path_tracing::extensions::images::image_info<path_tracing::real> path_tracing::extensions::images::read_image_sdr(
	const std::string& filename, const coordinate_system& system, bool gamma)
{
	auto channel = 0;
	auto size_x = 0;
	auto size_y = 0;

	image_info<real> info;

	const auto data = stbi_load(filename.c_str(), &size_x, &size_y, &channel, 
		STBI_rgb_alpha);

	info.size_x = size_x;
	info.size_y = size_y;
	info.values = std::vector<real>(info.size_x * info.size_y * 4);

	if (system == coordinate_system::left_hand) {
		
		for (size_t index = 0; index < info.values.size(); index++) {
			const auto value = static_cast<real>(data[index]) / 255;

			info.values[index] = gamma ? inverse_gamma_correct(value) : value;
		}
		
	}
	else {
		for (size_t y = 0; y < size_y; y++) {
			const auto invert_y = size_y - y - 1;

			for (size_t x = 0; x < size_x; x++) {
				const auto index0 = (y * size_x + x) * 4;
				const auto index1 = (invert_y * size_x + x) * 4;

				for (size_t component = 0; component < 4; component++) {
					const auto value = static_cast<real>(data[index1 + component]) / 255;

					info.values[index0 + component] = gamma ? inverse_gamma_correct(value) : value;
				}
			}
		}
	}

	stbi_image_free(data);

	return info;
}

path_tracing::extensions::images::image_info<path_tracing::real> path_tracing::extensions::images::read_image_hdr(
	const std::string& filename, const coordinate_system& system, bool gamma)
{
	auto channel = 0;
	auto size_x = 0;
	auto size_y = 0;

	image_info<real> info;

	const auto data = stbi_loadf(filename.c_str(), &size_x, &size_y, &channel, STBI_rgb_alpha);

	info.size_x = size_x;
	info.size_y = size_y;
	info.values = std::vector<real>(info.size_x * info.size_y * 4);

	if (system == coordinate_system::left_hand) {
		
		for (size_t index = 0; index < info.values.size(); index++)
			info.values[index] = gamma ? inverse_gamma_correct(data[index]) : data[index];
		
	}
	else {
		for (size_t y = 0; y < size_y; y++) {
			const auto invert_y = size_y - y - 1;

			for (size_t x = 0; x < size_x; x++) {
				const auto index0 = (y * size_x + x) * 4;
				const auto index1 = (invert_y * size_x + x) * 4;

				for (size_t component = 0; component < 4; component++) {
					const auto value = data[index1 + component];

					info.values[index0 + component] = gamma ? inverse_gamma_correct(value) : value;
				}
			}
		}
	}

	stbi_image_free(data);

	return info;
}
