#include "convert_texture.hpp"
#include "convert_spectrum.hpp"

#include "meta-scene/textures/constant_texture.hpp"


namespace path_tracing::core::converter {

	vector3 create_spectrum_texture(const std::shared_ptr<metascene::textures::constant_texture>& texture)
	{
		if (texture->value_type == metascene::textures::value_type::real)
			return vector3(texture->real);

		return read_spectrum(texture->spectrum);
	}
	
	vector3 create_spectrum_texture(const std::shared_ptr<metascene::textures::texture>& texture)
	{
		if (texture->type == metascene::textures::type::constant)
			return create_spectrum_texture(std::static_pointer_cast<metascene::textures::constant_texture>(texture));

		return vector3(0);
	}

	real create_real_texture(const std::shared_ptr<metascene::textures::constant_texture>& texture)
	{
		assert(texture->value_type == metascene::textures::value_type::real);

		return texture->real;
	}

	real create_real_texture(const std::shared_ptr<metascene::textures::texture>& texture)
	{
		if (texture->type == metascene::textures::type::constant)
			return create_real_texture(std::static_pointer_cast<metascene::textures::constant_texture>(texture));

		return 0;
	}

}
