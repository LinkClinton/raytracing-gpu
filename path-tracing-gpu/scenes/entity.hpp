#pragma once

#include "../runtime/resources/meshes_system.hpp"

#include "components/transform.hpp"

namespace path_tracing::scenes {

	using namespace components;
	
	using runtime::resources::mesh_info;
	
	struct entity final {
		transform transform;
		mesh_info mesh;

		entity() = default;
	};
	
}
