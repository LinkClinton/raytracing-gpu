#pragma once

#include "filter.hpp"

namespace metascene {

	namespace filters {

		struct gaussian_filter final : filter {
			real alpha = 2;
			
			gaussian_filter();
		};
		
	}
}
