#pragma once

#include "texture.hpp"

namespace metascene {

	namespace textures {

		struct scale_texture final : texture {
			std::shared_ptr<texture> scale;
			std::shared_ptr<texture> base;
			
			scale_texture();

			std::string to_string() const noexcept override;
		};

	}
}
