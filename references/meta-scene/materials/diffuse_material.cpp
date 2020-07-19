#include "diffuse_material.hpp"

metascene::materials::diffuse_material::diffuse_material() : material(type::diffuse)
{
}

std::string metascene::materials::diffuse_material::to_string() const noexcept
{
	std::stringstream stream;

	stream << "diffuse_material : " << std::endl;
	stream << "reflectance = " << reflectance->to_string() << std::endl;
	stream << "sigma = " << sigma->to_string() << std::endl;

	return stream.str();
}
