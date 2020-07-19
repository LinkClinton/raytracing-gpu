#pragma once

#include "meta-scene/entity.hpp"
#include "meta-scene/scene.hpp"

#include "../scenes/entity.hpp"
#include "../scenes/scene.hpp"

using namespace path_tracing::core::scenes;

namespace path_tracing::core::converter {

	std::shared_ptr<entity> create_entity(const std::shared_ptr<metascene::entity>& entity, real radius);

	std::shared_ptr<scene> create_scene(const std::shared_ptr<metascene::scene>& meta_scene);
	
}
