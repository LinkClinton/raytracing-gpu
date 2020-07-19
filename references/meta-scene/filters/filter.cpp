#include "filter.hpp"

metascene::filters::filter::filter(const filters::type& type, real radius_x, real radius_y) :
	type(type), radius_x(radius_x), radius_y(radius_y)
{
}
