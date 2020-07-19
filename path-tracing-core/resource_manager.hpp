#pragma once

#include "meta-scene/shapes/mesh.hpp"

#include "shapes/mesh.hpp"

#include <unordered_map>
#include <memory>

using namespace path_tracing::core::shapes;

namespace path_tracing::core {

	struct resource_manager final {
		static inline std::unordered_map<std::string, std::shared_ptr<mesh>> meshes;

		static std::shared_ptr<mesh> read_ply_mesh(const std::shared_ptr<metascene::shapes::mesh>& mesh);
	};
	
}
