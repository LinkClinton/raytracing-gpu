#pragma once

#include "../../runtime/resources/meshes_system.hpp"

namespace path_tracing::extensions::models {

	using runtime::resources::mesh_cpu_buffer;

	mesh_cpu_buffer generate_sphere_model(real radius, uint32 slice = 16, uint32 stack = 16);

}
