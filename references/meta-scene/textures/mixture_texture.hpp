#pragma once

#include "texture.hpp"

namespace metascene {

	namespace textures {

		struct mixture_texture final : texture {
			std::shared_ptr<texture> texture0;
			std::shared_ptr<texture> texture1;
			std::shared_ptr<texture> alpha;

			mixture_texture();

			std::string to_string() const noexcept override;
		};
		
	}
}
