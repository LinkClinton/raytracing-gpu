#pragma once

#include "../runtime/resources/meshes_system.hpp"

#include "components/submodule_data.hpp"
#include "components/transform.hpp"

#include <optional>

namespace path_tracing::scenes::components {

	using runtime::resources::mesh_info;
	
	struct submodule_mesh final {
		mesh_info info = mesh_info();

		bool reverse = false;

		submodule_mesh() = default;
	};
}

namespace path_tracing::scenes {

	using namespace components;

	using runtime::resources::mesh_info;

	struct entity final {
		std::optional<submodule_data> material;
		std::optional<submodule_data> light;
		std::optional<submodule_mesh> mesh;
		
		transform transform;
		
		entity() = default;
	};

	real compute_mesh_area_with_transform(const runtime::resources::meshes_system& system, 
		const transform& transform, const mesh_info& info);
	
}
