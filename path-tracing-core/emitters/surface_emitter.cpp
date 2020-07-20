#include "surface_emitter.hpp"

path_tracing::core::emitters::surface_emitter::surface_emitter(const vector3& radiance) :
	emitter(emitter_type::surface), mRadiance(radiance)
{
}

path_tracing::core::emitters::emitter_gpu_buffer path_tracing::core::emitters::surface_emitter::gpu_buffer(
	const transform& transform, uint32 index) const noexcept
{
	emitter_gpu_buffer buffer;

	buffer.intensity = mRadiance;
	buffer.type = type();
	buffer.delta = 0;
	buffer.index = index;

	return buffer;
}
