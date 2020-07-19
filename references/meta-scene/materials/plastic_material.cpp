#include "plastic_material.hpp"

metascene::materials::plastic_material::plastic_material() : material(type::plastic)
{
}

std::string metascene::materials::plastic_material::to_string() const noexcept
{
	std::stringstream stream;

	stream << "plastic_material : " << std::endl;
	stream << "specular  = " << specular->to_string() << std::endl;
	stream << "diffuse   = " << diffuse->to_string() << std::endl;
	stream << "roughness = " << roughness->to_string() << std::endl;
	stream << "eta       = " << eta->to_string() << std::endl;
	stream << "remapped  = " << remapped_roughness_to_alpha << std::endl;

	return stream.str();
}
