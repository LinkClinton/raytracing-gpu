#pragma once

#include "../../types.hpp"

namespace path_tracing::extensions::images {

	template <typename ValueType>
	struct image_info {
		std::vector<ValueType> values;

		uint32 size_x = 0;
		uint32 size_y = 0;

		image_info() = default;
	};
	
}
