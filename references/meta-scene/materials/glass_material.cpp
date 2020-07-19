#include "glass_material.hpp"

metascene::materials::glass_material::glass_material() : material(type::glass)
{
}

std::string metascene::materials::glass_material::to_string() const noexcept
{
	std::stringstream stream;

	stream << "glass material : " << std::endl;
	stream << "transmission = " << transmission->to_string() << std::endl;
	stream << "reflectance  = " << reflectance->to_string() << std::endl;
	stream << "roughness u  = " << roughness_u->to_string() << std::endl;
	stream << "roughness v  = " << roughness_v->to_string() << std::endl;
	stream << "eta          = " << eta->to_string() << std::endl;
	stream << "remapped     = " << remapped_roughness_to_alpha << std::endl;

	return stream.str();
}
