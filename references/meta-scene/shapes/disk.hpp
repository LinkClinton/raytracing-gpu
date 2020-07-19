#pragma once

#include "shape.hpp"

namespace metascene {

	namespace shapes {

		struct disk final : shape {
			real radius = static_cast<real>(1);
			real height = static_cast<real>(0);

			disk();

			std::string to_string() const noexcept override;
		};
		
	}
}
