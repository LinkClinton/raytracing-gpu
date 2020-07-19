#pragma once

#include "meta-scene/shapes/triangles.hpp"
#include "meta-scene/shapes/mesh.hpp"

#include "../shared/transform.hpp"

#include <string>

#define __PLY_IMPORTER__
#ifndef __PLY_IMPORTER__
#else

using namespace path_tracing::core::shared;

namespace path_tracing::core::importers {

	std::shared_ptr<metascene::shapes::triangles> load_ply_mesh(const std::string& filename);
	
}

#endif