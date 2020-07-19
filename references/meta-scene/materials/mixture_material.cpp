#include "mixture_material.hpp"

metascene::materials::mixture_material::mixture_material() : material(type::mixture)
{
}

std::string metascene::materials::mixture_material::to_string() const noexcept
{
	std::stringstream stream;

	stream << "mixture material : " << std::endl;
	stream << "material [0] = " << materials[0]->to_string() << std::endl;
	stream << "material [1] = " << materials[1]->to_string() << std::endl;
	stream << "alpha        = " << alpha->to_string() << std::endl;

	return stream.str();
}
