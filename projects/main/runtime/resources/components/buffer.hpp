#pragma once

#include "resource.hpp"

#include <directx12-wrapper/resources/buffer.hpp>

namespace raytracing::runtime::resources::components
{
	struct buffer_info final
	{
		size_t size = 0;
	};

	using cpu_buffer_data = std::vector<byte>;
	using gpu_buffer_data = wrapper::directx12::buffer;

	using cpu_buffer = resource<buffer_info, cpu_buffer_data>;
	using gpu_buffer = resource<buffer_info, gpu_buffer_data>;

	struct geometry_buffer_info final
	{
		uint32 vtx_count = 0;
		uint32 idx_count = 0;
	};

	struct geometry_buffer_data final
	{
		wrapper::directx12::buffer positions;
		wrapper::directx12::buffer normals;
		wrapper::directx12::buffer uvs;

		wrapper::directx12::buffer indices;
	};

	using geometry_buffer = resource<geometry_buffer_info, geometry_buffer_data>;
}
