#pragma once

#include "../textures/constant_texture.hpp"
#include "material.hpp"

#include <array>

namespace metascene {

	using namespace textures;
	
	namespace materials {

		struct mixture_material final : material {
			std::array<std::shared_ptr<material>, 2> materials = { nullptr, nullptr };

			std::shared_ptr<texture> alpha = std::make_shared<constant_texture>(value_type::real);

			mixture_material();

			std::string to_string() const noexcept override;
		};
		
	}
}
