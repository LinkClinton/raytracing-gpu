#include "uber_material.hpp"

metascene::materials::uber_material::uber_material() : material(type::uber)
{
}

std::string metascene::materials::uber_material::to_string() const noexcept
{
	std::stringstream stream;

	stream << "uber_material : " << std::endl;
	stream << "specular     = " << specular->to_string() << std::endl;
	stream << "diffuse      = " << diffuse->to_string() << std::endl;
	stream << "transmission = " << transmission->to_string() << std::endl;
	stream << "reflectance  = " << reflectance->to_string() << std::endl;
	stream << "opacity      = " << opacity->to_string() << std::endl;
	stream << "roughness_u  = " << roughness_u->to_string() << std::endl;
	stream << "roughness_v  = " << roughness_v->to_string() << std::endl;
	stream << "eta          = " << eta->to_string() << std::endl;
	stream << "remapped     = " << remapped_roughness_to_alpha << std::endl;

	return stream.str();
}
