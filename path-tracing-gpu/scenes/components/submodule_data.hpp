#pragma once

#include "../../types.hpp"

namespace path_tracing::scenes::components {

	struct texture {
		vector3 value = vector3(1);
		uint32 index = index_null;

		texture() = default;
	};
	
	struct submodule_data final {
		mapping<std::string, texture> texture;
		mapping<std::string, vector3> float3;
		
		mapping<std::string, uint32> uint;

		mapping<std::string, real> real;
		
		std::string submodule;
		
		submodule_data() = default;
	};

	size_t size_of_submodule_data_type(const std::string& type);
	
}
