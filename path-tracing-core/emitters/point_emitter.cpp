#include "point_emitter.hpp"

path_tracing::core::emitters::point_emitter::point_emitter(const vector3& intensity) :
	emitter(emitter_type::point), mIntensity(intensity)
{	
}

path_tracing::core::emitters::emitter_gpu_buffer path_tracing::core::emitters::point_emitter::gpu_buffer(
	const transform& transform) const noexcept
{
	emitter_gpu_buffer buffer;

	buffer.intensity = mIntensity;
	buffer.position = transform_point(transform, vector3(0));
	buffer.type = type();

	return buffer;
}
