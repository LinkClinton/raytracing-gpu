#pragma once

#include "../../runtime/resources/meshes_system.hpp"

namespace path_tracing::extensions::models {

	using runtime::resources::mesh_cpu_buffer;

	mesh_cpu_buffer load_ply_mesh(const std::string& filename);

}
