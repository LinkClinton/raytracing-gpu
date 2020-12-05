#pragma once

#include "../../scenes/components/submodule_data.hpp"
#include "../../runtime/resources/meshes_system.hpp"
#include "../../runtime/resources/images_system.hpp"

#include "json.hpp"

#include <optional>

namespace path_tracing::extensions::json {

	using scenes::components::submodule_data;
	using runtime::resources::images_system;
	using runtime::resources::mesh_info;
	
	submodule_data load_material_from_json(const nlohmann::json& material);

	submodule_data load_light_from_json(images_system& system, const nlohmann::json& light, const std::string& directory, uint32 index);
	
}
