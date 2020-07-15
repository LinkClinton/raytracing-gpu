#include "scene_pipeline.hpp"

void path_tracing::dx::utilities::scene_pipeline::set_resource_scene(const std::shared_ptr<resource_scene>& resource)
{
	mResourceScene = resource;
}

void path_tracing::dx::utilities::scene_pipeline::set_entities(const std::vector<std::shared_ptr<entity>>& entities)
{
	mEntities = entities;
}

void path_tracing::dx::utilities::scene_pipeline::set_max_depth(size_t depth)
{
	mMaxDepth = depth;
}

void path_tracing::dx::utilities::scene_pipeline::execute(const std::shared_ptr<device>& device)
{
	mPipeline = std::make_shared<raytracing_pipeline>();

	// todo:
}
