#include "surface_emitter.hpp"

metascene::emitters::surface_emitter::surface_emitter() : emitter(type::surface)
{
}

std::string metascene::emitters::surface_emitter::to_string() const noexcept
{
	std::stringstream stream;

	stream << "surface_emitter : " << std::endl;
	stream << "radiance = " << radiance->to_string() << std::endl;

	return stream.str();
}
