#include "camera.hpp"

path_tracing::core::cameras::camera_gpu_buffer::camera_gpu_buffer(
	const shared::transform& projective,
	const shared::transform& transform, 
	real focus, real lens) :
	projective(projective), transform(transform), focus(focus), lens(lens)
{
}

path_tracing::core::cameras::camera::camera(
	const shared::transform& projective, 
	const shared::transform& transform,
	real focus, real lens) :
	mProjective(projective), mTransform(transform),
	mFocus(focus), mLens(lens)
{
}

path_tracing::core::shared::transform path_tracing::core::cameras::camera::projective() const noexcept
{
	return mProjective;
}

path_tracing::core::shared::transform path_tracing::core::cameras::camera::transform() const noexcept
{
	return mTransform;
}

path_tracing::core::cameras::camera_gpu_buffer path_tracing::core::cameras::camera::gpu_buffer() const noexcept
{
	return camera_gpu_buffer(mProjective, mTransform, mFocus, mLens);
}
