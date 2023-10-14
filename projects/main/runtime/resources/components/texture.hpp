#pragma once

#include "../../../types.hpp"

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

		uint32 channel = 1;

		texture_format format = texture_format::uint8;
	};

	using texture_data = std::vector<byte>;

	struct texture final
	{
		texture_info info;
		texture_data data;
	};
}
