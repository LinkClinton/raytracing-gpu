#include "mixture_texture.hpp"

metascene::textures::mixture_texture::mixture_texture() : texture(type::mixture)
{
}

std::string metascene::textures::mixture_texture::to_string() const noexcept
{
	std::stringstream stream;

	stream << "mixture_texture : " << std::endl;
	stream << "texture0 = " << texture0->to_string() << std::endl;
	stream << "texture1 = " << texture1->to_string() << std::endl;
	stream << "alpha    = " << alpha->to_string() << std::endl;
	
	return stream.str();
}
