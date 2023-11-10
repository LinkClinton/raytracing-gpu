#pragma once

#include "resource.hpp"

#include <directx12-wrapper/resources/texture2d.hpp>

namespace raytracing::runtime::resources::components
{
	enum class texture_format : uint32
	{
		// unsigned normalized integer, uniform mapping [0, 255] -> [0.0, 1.0]
		uint8 = 0,
		// 32bit float point number
		fp32 = 1
	};

	struct texture_info final
	{
		uint32 size_x = 0;
		uint32 size_y = 0;

		uint32 channel = 0;

		texture_format format = texture_format::uint8;
	};

	using cpu_texture_data = std::vector<byte>;
	using gpu_texture_data = wrapper::directx12::texture2d;

	using cpu_texture = resource<texture_info, cpu_texture_data>;
	using gpu_texture = resource<texture_info, gpu_texture_data>;
}
