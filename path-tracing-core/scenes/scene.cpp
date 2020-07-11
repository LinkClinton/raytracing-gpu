#include "scene.hpp"

void path_tracing::core::scenes::scene::add_entity(const std::shared_ptr<entity>& entity)
{
	if (entity->has_component<emitter>())
		mEmitters.push_back(entity);
	
	mEntities.push_back(entity);
}

const std::vector<std::shared_ptr<path_tracing::core::scenes::entity>>& path_tracing::core::scenes::scene::entities() const noexcept
{
	return mEntities;
}

const std::vector<std::shared_ptr<path_tracing::core::scenes::entity>>& path_tracing::core::scenes::scene::emitters() const noexcept
{
	return mEmitters;
}
