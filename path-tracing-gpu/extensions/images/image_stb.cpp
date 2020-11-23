#include "image_stb.hpp"

#include "../../runtime/render/render_device.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>

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
