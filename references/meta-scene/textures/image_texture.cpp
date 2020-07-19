#include "image_texture.hpp"

metascene::textures::image_texture::image_texture() : texture(type::image)
{
}

std::string metascene::textures::image_texture::to_string() const noexcept
{
	std::stringstream stream;

	stream << "image_texture : " << std::endl;
	stream << "filename = " << filename << std::endl;
	stream << "gamma    = " << gamma << std::endl;

	return stream.str();
}