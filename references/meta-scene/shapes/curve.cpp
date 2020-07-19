#include "curve.hpp"

metascene::shapes::curve::curve() : shape(type::curve)
{
}

std::string metascene::shapes::curve::to_string() const noexcept
{
	std::stringstream stream;
	
	stream << "curve : " << std::endl;
	stream << "width = " << width[0] << ", " << width[1] << std::endl;
	stream << "u_min = " << u_min << ", u_max = " << u_max << std::endl;
	stream << "reverse_orientation = " << reverse_orientation << std::endl;

	return stream.str();
}
