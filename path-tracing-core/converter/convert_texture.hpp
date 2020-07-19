#pragma once

#include "meta-scene/textures/texture.hpp"

#include "../shared/transform.hpp"

using namespace path_tracing::core::shared;

namespace path_tracing::core::converter {

	vector3 create_spectrum_texture(const std::shared_ptr<metascene::textures::texture>& texture);

	real create_real_texture(const std::shared_ptr<metascene::textures::texture>& texture);
	
}
