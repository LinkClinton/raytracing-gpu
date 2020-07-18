#include "entity.hpp"

path_tracing::core::scenes::entity_gpu_buffer::entity_gpu_buffer(const matrix4x4& transform, uint32 material, uint32 emitter, uint32 shape) :
	local_to_world(transform), material(material), emitter(emitter), shape(shape)
{
}

path_tracing::core::scenes::entity::entity(
	const std::shared_ptr<material>& material,
	const std::shared_ptr<emitter>& emitter, 
	const std::shared_ptr<shape>& shape, 
	const shared::transform& transform) :
	mMaterial(material), mEmitter(emitter), mShape(shape), mTransform(transform)
{
}

path_tracing::core::shared::transform path_tracing::core::scenes::entity::transform() const noexcept
{
	return mTransform;
}

path_tracing::core::scenes::entity_gpu_buffer path_tracing::core::scenes::entity::gpu_buffer(uint32 material, uint32 emitter, uint32 shape) const noexcept
{
	return entity_gpu_buffer(transpose(mTransform.matrix), material, emitter, shape);
}
