#include "scale_texture.hpp"

metascene::textures::scale_texture::scale_texture() : texture(type::scale)
{
}

std::string metascene::textures::scale_texture::to_string() const noexcept
{
	std::stringstream stream;

	stream << "scale_texture : " << std::endl;
	stream << "scale = " << scale->to_string() << std::endl;
	stream << "base  = " << base->to_string() << std::endl;

	return stream.str();
}