#pragma once

#include "../utilities.hpp"

namespace metascene {

	namespace filters {

		enum class type : uint32 {
			unknown = 0, box = 1, gaussian = 2
		};
		
		struct filter {
			type type = type::unknown;

			real radius_x = 0.5;
			real radius_y = 0.5;
			
			filter() = default;

			filter(const filters::type& type, real radius_x, real radius_y);
		};
		
	}
}
