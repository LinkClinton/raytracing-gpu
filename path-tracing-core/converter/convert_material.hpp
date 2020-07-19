#pragma once

#include "meta-scene/materials/material.hpp"

#include "../materials/material.hpp"

using namespace path_tracing::core::materials;

namespace path_tracing::core::converter {

	std::shared_ptr<material> create_material(const std::shared_ptr<metascene::materials::material>& material);
	
}
