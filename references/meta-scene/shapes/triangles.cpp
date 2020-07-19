#include "triangles.hpp"

metascene::shapes::triangles::triangles() : shape(type::triangles)
{
}

std::string metascene::shapes::triangles::to_string() const noexcept
{
	std::stringstream stream;

	stream << "triangles : " << std::endl;
	stream << "positions = " << positions.size() << std::endl;
	stream << "normals   = " << normals.size() << std::endl;
	stream << "uvs       = " << uvs.size() << std::endl;
	stream << "mask      = " << (mask == nullptr ? "nullptr" : mask->to_string()) << std::endl;
	stream << "reverse_orientation = " << reverse_orientation << std::endl;
	
	return stream.str();
}
