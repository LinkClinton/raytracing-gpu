#include "sphere.hpp"

path_tracing::core::shapes::sphere::sphere(real radius, bool reverse_orientation) :
	shape(reverse_orientation), mRadius(radius)
{
}

path_tracing::core::real path_tracing::core::shapes::sphere::radius() const noexcept
{
	return mRadius;
}
