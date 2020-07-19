#include "environment_emitter.hpp"

metascene::emitters::environment_emitter::environment_emitter() : emitter(type::environment)
{
}

std::string metascene::emitters::environment_emitter::to_string() const noexcept
{
	std::stringstream stream;

	stream << "environment_emitter : " << std::endl;
	stream << "intensity = " << intensity->to_string() << std::endl;
	stream << "environment_map = " << environment_map << std::endl;
	stream << "gamma = " << gamma << std::endl;

	return stream.str();
}
