#include "environment_emitter.hpp"

path_tracing::core::emitters::environment_emitter::environment_emitter(const std::shared_ptr<texture>& map, const vector3& intensity) :
	emitter(emitter_type::environment), mMap(map), mIntensity(intensity)
{
}

path_tracing::core::emitters::environment_emitter::environment_emitter(const vector3& intensity) :
	emitter(emitter_type::environment), mIntensity(intensity)
{
}

path_tracing::core::emitters::emitter_gpu_buffer path_tracing::core::emitters::environment_emitter::gpu_buffer(
	const transform& transform, uint32 index) const noexcept
{
	emitter_gpu_buffer buffer;

	buffer.index = index;
	buffer.intensity = mIntensity;
	buffer.delta = 0;
	buffer.type = type();

	return buffer;
}
