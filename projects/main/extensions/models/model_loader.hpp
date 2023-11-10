#pragma once

#include "../../runtime/resources/components/mesh.hpp"

namespace raytracing::extensions::models
{

	runtime::resources::components::cpu_mesh load_mesh(const std::string& filename);

}
