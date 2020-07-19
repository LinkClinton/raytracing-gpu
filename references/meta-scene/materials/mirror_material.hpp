#pragma once

#include "../textures/constant_texture.hpp"
#include "material.hpp"

namespace metascene {

	using namespace textures;
	
	namespace materials {

		struct mirror_material final : material {
			std::shared_ptr<texture> reflectance = std::make_shared<constant_texture>(value_type::spectrum);

			mirror_material();

			std::string to_string() const noexcept override;
		};
		
	}
}
