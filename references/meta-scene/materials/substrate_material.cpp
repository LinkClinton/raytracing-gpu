#include "substrate_material.hpp"

metascene::materials::substrate_material::substrate_material() : material(type::substrate)
{
}

std::string metascene::materials::substrate_material::to_string() const noexcept
{
	std::stringstream stream;

	stream << "substrate_material : " << std::endl;
	stream << "specular    = " << specular->to_string() << std::endl;
	stream << "diffuse     = " << diffuse->to_string() << std::endl;
	stream << "roughness_u = " << roughness_u->to_string() << std::endl;
	stream << "roughness_v = " << roughness_v->to_string() << std::endl;
	stream << "remapped    = " << remapped_roughness_to_alpha << std::endl;

	return stream.str();
}
