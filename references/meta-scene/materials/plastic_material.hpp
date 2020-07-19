#pragma once

#include "../textures/constant_texture.hpp"
#include "material.hpp"

#include <memory>

namespace metascene {

	using namespace textures;
	
	namespace materials {

		struct plastic_material final : material {
			std::shared_ptr<texture> specular = std::make_shared<constant_texture>(value_type::spectrum);
			std::shared_ptr<texture> diffuse = std::make_shared<constant_texture>(value_type::spectrum);

			std::shared_ptr<texture> roughness = std::make_shared<constant_texture>(value_type::real);
			std::shared_ptr<texture> eta = std::make_shared<constant_texture>(value_type::real);
			
			bool remapped_roughness_to_alpha = true;
			
			plastic_material();

			std::string to_string() const noexcept override;
		};
		
	}
}
