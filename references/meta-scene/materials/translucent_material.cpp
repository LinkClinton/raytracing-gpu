#include "translucent_material.hpp"

metascene::materials::translucent_material::translucent_material() : material(type::translucent)
{
}

std::string metascene::materials::translucent_material::to_string() const noexcept
{
	std::stringstream stream;

	stream << "translucent material : " << std::endl;
	stream << "transmission = " << transmission->to_string() << std::endl;
	stream << "reflectance  = " << reflectance->to_string() << std::endl;
	stream << "specular     = " << specular->to_string() << std::endl;
	stream << "diffuse      = " << diffuse->to_string() << std::endl;
	stream << "roughness    = " << roughness->to_string() << std::endl;
	stream << "remapped     = " << remapped_roughness_to_alpha << std::endl;

	return stream.str();
}
