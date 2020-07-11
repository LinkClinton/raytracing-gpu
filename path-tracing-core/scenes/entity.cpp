#include "entity.hpp"

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
