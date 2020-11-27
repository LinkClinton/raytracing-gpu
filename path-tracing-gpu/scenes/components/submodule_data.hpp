#pragma once

#include "../../types.hpp"

namespace path_tracing::scenes::components {

	struct submodule_data final {
		mapping<std::string, std::vector<byte>> properties;

		std::string submodule;

		submodule_data() = default;
	};
	
}
