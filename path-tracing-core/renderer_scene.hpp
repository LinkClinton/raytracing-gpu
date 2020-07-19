#pragma once

#include "meta-scene/scene.hpp"

#include "cameras/camera.hpp"
#include "scenes/scene.hpp"

using namespace path_tracing::core::cameras;
using namespace path_tracing::core::scenes;

namespace path_tracing::core {

	struct renderer_scene final {
		std::shared_ptr<camera> camera;
		std::shared_ptr<scene> scene;

		size_t width = 0;
		size_t height = 0;

		renderer_scene() = default;
	};

	renderer_scene create_from_meta_scene(const std::shared_ptr<metascene::scene>& meta_scene);
	
}
