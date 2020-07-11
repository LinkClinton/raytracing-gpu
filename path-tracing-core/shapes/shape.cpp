#include "shape.hpp"

path_tracing::core::shapes::shape::shape(bool reverse_orientation) :
	mReverseOrientation(reverse_orientation)
{
}

bool path_tracing::core::shapes::shape::reverse_orientation() const noexcept
{
	return mReverseOrientation;
}
