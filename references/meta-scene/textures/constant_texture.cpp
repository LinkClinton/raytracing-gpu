#include "constant_texture.hpp"

metascene::textures::constant_texture::constant_texture() : texture(type::constant)
{
}

metascene::textures::constant_texture::constant_texture(metascene::real value) :
	texture(type::constant), real(value), value_type(value_type::real)
{
}

metascene::textures::constant_texture::constant_texture(const std::shared_ptr<spectrums::spectrum>& spectrum) :
	texture(type::constant), spectrum(spectrum), value_type(value_type::spectrum)
{
}

metascene::textures::constant_texture::constant_texture(const textures::value_type& value_type) :
	texture(type::constant), value_type(value_type)
{
}

std::string metascene::textures::constant_texture::to_string() const noexcept
{
	std::stringstream stream;

	stream << "constant_texture : " << std::endl;
	stream << "value_type = " << textures::to_string(value_type) << std::endl;
	stream << "spectrum = " << spectrum->to_string() << std::endl;
	stream << "real     = " << real << std::endl;

	return stream.str();
}

std::string metascene::textures::to_string(const value_type& type)
{
	switch (type) {
	case value_type::spectrum: return "spectrum";
	case value_type::real: return "real";
	default: return "unknown";
	}
}
