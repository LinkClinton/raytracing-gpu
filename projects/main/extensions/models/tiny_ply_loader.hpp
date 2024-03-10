#pragma once

#include "../../runtime/resources/components/mesh.hpp"

namespace raytracing::extensions::models
{
	runtime::resources::components::mesh load_ply_mesh(const std::string& filename);
}
