#include "homogeneous_medium.hpp"

metascene::media::homogeneous_medium::homogeneous_medium() : medium(type::homogeneous)
{
}

std::string metascene::media::homogeneous_medium::to_string() const noexcept
{
	std::stringstream stream;

	stream << "homogeneous medium : " << std::endl;
	stream << "sigma_a = " << sigma_a->to_string() << std::endl;
	stream << "sigma_s = " << sigma_s->to_string() << std::endl;
	stream << "g       = " << g << std::endl;

	return stream.str();
}
