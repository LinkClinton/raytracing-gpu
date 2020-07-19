#include "directional_emitter.hpp"

metascene::emitters::directional_emitter::directional_emitter() : emitter(type::directional)
{
}

std::string metascene::emitters::directional_emitter::to_string() const noexcept
{
	std::stringstream stream;

	stream << "directional emitter : " << std::endl;
	stream << "irradiance = " << irradiance->to_string() << std::endl;
	stream << "from       = " << math::to_string(from) << std::endl;
	stream << "to         = " << math::to_string(to) << std::endl;

	return stream.str();
}
