#pragma once

#include "meta-scene/cameras/camera.hpp"
#include "meta-scene/cameras/film.hpp"

#include "../cameras/camera.hpp"

using namespace path_tracing::core::cameras;

namespace path_tracing::core::converter {

	std::shared_ptr<camera> create_camera(
		const std::shared_ptr<metascene::cameras::camera>& camera,
		const std::shared_ptr<metascene::cameras::film>& film);

	
}
