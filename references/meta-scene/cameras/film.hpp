#pragma once

#include "../filters/box_filter.hpp"
#include "../filters/filter.hpp"

#include <memory>

namespace metascene {

	using namespace filters;
	
	namespace cameras {

		struct film {
			std::shared_ptr<filter> filter = std::make_shared<box_filter>();
			
			size_t height = 720;
			size_t width = 1280;
			real scale = 1;
			
			film() = default;
		};
		
	}
}
