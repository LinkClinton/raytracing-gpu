#include "scene_pipeline.hpp"

path_tracing::dx::utilities::scene_pipeline::scene_pipeline(const std::shared_ptr<device>& device) :
	mDevice(device)
{
	mCommandAllocator = std::make_shared<command_allocator>(mDevice);
	mCommandList = std::make_shared<graphics_command_list>(mCommandAllocator, mDevice);
}

void path_tracing::dx::utilities::scene_pipeline::set_resource_cache(const std::shared_ptr<resource_cache>& cache)
{
	mResourceCache = cache;
}

void path_tracing::dx::utilities::scene_pipeline::set_resource_scene(const std::shared_ptr<resource_scene>& scene)
{
	mResourceScene = scene;
}

void path_tracing::dx::utilities::scene_pipeline::set_entities(const std::vector<std::shared_ptr<entity>>& entities)
{
	mEntities = entities;
}

void path_tracing::dx::utilities::scene_pipeline::set_max_depth(size_t depth)
{
	mMaxDepth = depth;
}

void path_tracing::dx::utilities::scene_pipeline::execute(const std::shared_ptr<command_queue>& queue)
{
	const std::wstring ray_generation_shader = L"ray_generation_shader";
	const std::vector<std::wstring> miss_shaders = { L"miss_shader" };
	const shader_raytracing_config config = { D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES, 32 };
	
	mPipeline = std::make_shared<raytracing_pipeline>();

	std::vector<shader_association> associations;
	std::vector<hit_group> groups;

	for (const auto& entity : mEntities) {
		const auto entity_cache = mResourceCache->cache(entity);

		if (!entity_cache.group.has_value()) continue;

		associations.push_back(entity_cache.association.value());
		groups.push_back(entity_cache.group.value());
	}

	associations.push_back(shader_association(nullptr, config, ray_generation_shader));

	for (size_t index = 0; index < miss_shaders.size(); index++) 
		associations.push_back(shader_association(nullptr, config, miss_shaders[index]));

	// todo: set shader libraries
	mPipeline->set_shader_associations(associations);
	mPipeline->set_hit_group_shaders(groups);
	mPipeline->set_miss_shaders(miss_shaders);
	mPipeline->set_root_signature(mResourceScene->signature());
	mPipeline->set_ray_generation_shader(ray_generation_shader);
	mPipeline->set_max_depth(mMaxDepth);

	mPipeline->build(mDevice);

	const auto shader_table = mPipeline->shader_table();

	shader_table->begin_mapping();

	for (const auto& entity : mEntities) {
		const auto entity_cache = mResourceCache->cache(entity);

		if (!entity_cache.group.has_value()) continue;

		std::memcpy(shader_table->shader_record_address(entity_cache.group->name) +
			D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,
			entity_cache.shader_table_data.data(),
			entity_cache.shader_table_data.size());
	}
	
	shader_table->end_mapping();

	(*mCommandAllocator)->Reset();
	(*mCommandList)->Reset(mCommandAllocator->get(), nullptr);

	shader_table->upload(mCommandList);

	(*mCommandList)->Close();

	queue->execute({ mCommandList });
	queue->wait();
}
