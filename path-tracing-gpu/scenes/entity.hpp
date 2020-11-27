#pragma once

#include "../runtime/resources/meshes_system.hpp"

#include "components/submodule_data.hpp"
#include "components/transform.hpp"

namespace path_tracing::scenes {

	using namespace components;
	
	using runtime::resources::mesh_info;
	
	struct entity final {
		submodule_data material;
		submodule_data light;
		
		transform transform;
		mesh_info mesh;

		entity() = default;
	};
	
}
