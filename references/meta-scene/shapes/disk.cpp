#include "disk.hpp"

metascene::shapes::disk::disk() : shape(type::disk)
{
}

std::string metascene::shapes::disk::to_string() const noexcept
{
	std::stringstream stream;

	stream << "disk : " << std::endl;
	stream << "radius = " << radius << std::endl;
	stream << "height = " << height << std::endl;
	stream << "reverse_orientation = " << reverse_orientation << std::endl;
	
	return stream.str();
}
