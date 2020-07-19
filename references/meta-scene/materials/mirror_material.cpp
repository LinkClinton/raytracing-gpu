#include "mirror_material.hpp"

metascene::materials::mirror_material::mirror_material() : material(type::mirror)
{
}

std::string metascene::materials::mirror_material::to_string() const noexcept
{
	std::stringstream stream;

	stream << "mirror_material : " << std::endl;
	stream << "reflectance = " << reflectance->to_string() << std::endl;

	return stream.str();
}
