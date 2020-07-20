#include "entity.hpp"

path_tracing::core::scenes::entity::entity(
	const std::shared_ptr<material>& material,
	const std::shared_ptr<emitter>& emitter, 
	const std::shared_ptr<shape>& shape, 
	const shared::transform& transform) :
	indexable(mIndexCount++), mMaterial(material), mEmitter(emitter), mShape(shape), mTransform(transform)
{
}

path_tracing::core::shared::transform path_tracing::core::scenes::entity::transform() const noexcept
{
	return mTransform;
}

path_tracing::core::scenes::entity_gpu_buffer path_tracing::core::scenes::entity::gpu_buffer() const noexcept
{
	entity_gpu_buffer buffer;

	buffer.local_to_world = transpose(mTransform.matrix);
	buffer.world_to_local = transpose(mTransform.inverse);
	buffer.material = mMaterial == nullptr ? entity_gpu_buffer::null : mMaterial->index();
	buffer.emitter = mEmitter == nullptr ? entity_gpu_buffer::null : mEmitter->index();
	buffer.shape = mShape == nullptr ? entity_gpu_buffer::null : mShape->index();
	buffer.area = mShape == nullptr ? 0 : mShape->area(mTransform);

	return buffer;
}

path_tracing::core::uint32 path_tracing::core::scenes::entity::count() noexcept
{
	return mIndexCount;
}
