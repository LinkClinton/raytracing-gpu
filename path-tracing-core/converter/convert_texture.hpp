#pragma once

#include "meta-scene/textures/texture.hpp"

#include "../textures/texture.hpp"
#include "../shared/transform.hpp"

using namespace path_tracing::core::textures;
using namespace path_tracing::core::shared;

namespace path_tracing::core::converter {

	vector3 create_constant_spectrum_texture(const std::shared_ptr<metascene::textures::texture>& texture);

	real create_constant_real_texture(const std::shared_ptr<metascene::textures::texture>& texture);

	std::shared_ptr<texture> create_image_spectrum_texture(const std::shared_ptr<metascene::textures::texture>& texture);

	std::shared_ptr<texture> create_image_real_texture(const std::shared_ptr<metascene::textures::texture>& texture);
}
