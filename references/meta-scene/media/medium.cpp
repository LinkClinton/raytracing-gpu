#include "medium.hpp"

metascene::media::medium::medium(const metascene::media::type& type) : type(type)
{
}

std::string metascene::media::media::to_string() const noexcept
{
	std::stringstream stream;

	stream << "media : " << std::endl;
	stream << "outside = " << (outside == nullptr ? "vacuum" : outside->to_string());
	stream << "inside  = " << (inside == nullptr ? "vacuum" : inside->to_string());

	return stream.str();
}
