#pragma once

#include "../interfaces/string_property.hpp"
#include "../utilities.hpp"

namespace metascene {

	namespace textures {

		enum class type : uint32 {
			unknown = 0, image = 1, scale = 2, constant = 3, mixture = 4
		};

		struct texture : interfaces::string_property {
			type type = type::unknown;

			texture() = default;

			texture(const textures::type& type);
		};
	}
}
