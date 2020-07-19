#pragma once

#include "../spectrums/color_spectrum.hpp"
#include "texture.hpp"

namespace metascene {

	using namespace spectrums;
	
	namespace textures {

		enum class value_type : uint32 {
			unknown = 0, spectrum = 1, real = 2
		};

		struct constant_texture final : texture {
			std::shared_ptr<spectrum> spectrum = std::make_shared<color_spectrum>();

			real real = static_cast<metascene::real>(0);

			value_type value_type = value_type::unknown;

			constant_texture();

			constant_texture(metascene::real value);

			constant_texture(const std::shared_ptr<spectrums::spectrum>& spectrum);
			
			constant_texture(const textures::value_type& value_type);
			
			std::string to_string() const noexcept override;
		};

		std::string to_string(const value_type& type);
	}
}
