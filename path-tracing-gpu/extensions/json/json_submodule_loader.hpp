#pragma once

#include "../../scenes/components/submodule_data.hpp"
#include "../../runtime/resources/meshes_system.hpp"
#include "../../runtime/resources/images_system.hpp"

#include "json.hpp"

#include <optional>

namespace path_tracing::extensions::json {

	using scenes::components::submodule_data;
	using runtime::resources::images_system;
	using runtime::resources::meshes_system;
	using runtime::resources::mesh_info;

	struct resource_context final {
		images_system& images_system;
		meshes_system& meshes_system;

		coordinate_system coordinate;
		
		std::string directory;
	};
	
	submodule_data load_material_from_json(const resource_context& context, const nlohmann::json& material);

	submodule_data load_light_from_json(const resource_context& context, const nlohmann::json& light, uint32 index);
	
}
