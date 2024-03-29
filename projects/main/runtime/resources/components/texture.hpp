#pragma once

#include "resource.hpp"

#include <directx12-wrapper/resources/texture2d.hpp>

namespace raytracing::runtime::resources::components
{
	enum class texture_format : uint32
	{
		// unsigned normalized integer, uniform mapping [0, 255] -> [0.0, 1.0]
		fp8,
		// 32bit float point number(IEEE 754)
		fp32
	};

	struct texture_info final
	{
		uint32 size_x = 0;
		uint32 size_y = 0;

		uint32 channel = 0;

		texture_format format = texture_format::fp8;
	};

	using cpu_texture_data = std::vector<byte>;
	using gpu_texture_data = wrapper::directx12::texture2d;

	using cpu_texture = resource<texture_info, cpu_texture_data>;
	using gpu_texture = resource<texture_info, gpu_texture_data>;

	inline DXGI_FORMAT to_dxgi_format(texture_format format, uint32 channel)
	{
		using channel_table = std::array<DXGI_FORMAT, 5>;

		static mapping<texture_format, channel_table> dxgi_format_table =
		{
			{
				texture_format::fp8,
				channel_table
				{
					DXGI_FORMAT_UNKNOWN,
					DXGI_FORMAT_R8_UNORM,
					DXGI_FORMAT_R8G8_UNORM,
					DXGI_FORMAT_R8G8B8A8_UNORM,
					DXGI_FORMAT_R8G8B8A8_UNORM
				}
			},
			{
				texture_format::fp32,
				channel_table
				{
					DXGI_FORMAT_UNKNOWN,
					DXGI_FORMAT_R32_FLOAT,
					DXGI_FORMAT_R32G32_FLOAT,
					DXGI_FORMAT_R32G32B32_FLOAT,
					DXGI_FORMAT_R32G32B32A32_FLOAT
				}
			}
		};

		return dxgi_format_table[format][channel];
	}
}
