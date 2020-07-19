#pragma once

#include "texture.hpp"

namespace metascene {

	namespace textures {

		struct image_texture final : texture {
			std::string filename;

			bool gamma = false;

			image_texture();

			std::string to_string() const noexcept override;
		};
		
	}
}
