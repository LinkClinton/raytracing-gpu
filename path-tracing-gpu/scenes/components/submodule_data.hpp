#pragma once

#include "../../types.hpp"

namespace path_tracing::scenes::components {

	struct submodule_data final {
		mapping<std::string, vector3> float3;
		
		mapping<std::string, uint32> texture;
		mapping<std::string, uint32> uint;

		mapping<std::string, real> real;
		
		std::string submodule;
		
		submodule_data() = default;
	};

}
