#pragma once

#include "../textures/constant_texture.hpp"
#include "material.hpp"

namespace metascene {

	using namespace textures;
	
	namespace materials {

		struct substrate_material final : material {
			std::shared_ptr<texture> specular = std::make_shared<constant_texture>(value_type::spectrum);
			std::shared_ptr<texture> diffuse = std::make_shared<constant_texture>(value_type::spectrum);

			std::shared_ptr<texture> roughness_u = std::make_shared<constant_texture>(value_type::real);
			std::shared_ptr<texture> roughness_v = std::make_shared<constant_texture>(value_type::real);

			bool remapped_roughness_to_alpha = true;

			substrate_material();

			std::string to_string() const noexcept override;
		};
		
	}
}
