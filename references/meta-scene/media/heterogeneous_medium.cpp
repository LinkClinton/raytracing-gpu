#include "heterogeneous_medium.hpp"

metascene::media::heterogeneous_medium::heterogeneous_medium() : medium(type::heterogeneous)
{
}

std::string metascene::media::heterogeneous_medium::to_string() const noexcept
{
	std::stringstream stream;

	stream << "heterogeneous medium : " << std::endl;
	stream << "albedo = " << albedo->to_string() << std::endl;
	stream << "size_x = " << x << std::endl;
	stream << "size_y = " << y << std::endl;
	stream << "size_z = " << z << std::endl;
	stream << "g      = " << g << std::endl;

	return stream.str();
}
