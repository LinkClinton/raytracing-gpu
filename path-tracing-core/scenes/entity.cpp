#include "entity.hpp"

path_tracing::core::scenes::entity_gpu_buffer::entity_gpu_buffer(
	const matrix4x4& local_to_world, const matrix4x4& world_to_local, 
	uint32 material, uint32 emitter, uint32 shape, real area) :
	local_to_world(local_to_world), world_to_local(world_to_local), material(material), emitter(emitter), shape(shape), area(area)
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
	return entity_gpu_buffer(
		transpose(mTransform.matrix), transpose(mTransform.inverse), material, emitter, shape, 
		mShape == nullptr ? 0 : mShape->area(mTransform));
}
