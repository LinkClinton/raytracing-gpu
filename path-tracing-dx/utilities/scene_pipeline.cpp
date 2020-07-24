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

void path_tracing::dx::utilities::scene_pipeline::execute(const std::shared_ptr<command_queue>& queue)
{
	const std::wstring ray_generation_shader = L"ray_generation_shader";
	const std::vector<std::wstring> miss_shaders = { L"miss_shader" };
	const shader_raytracing_config config = { 8, shader_raytracing_config::default_payload_size };

	std::unordered_map<std::wstring, std::vector<std::wstring>> shaders_map;

	shaders_map.insert({ L"./shaders/ray_generation.hlsl", {
		ray_generation_shader
	} });

	shaders_map.insert({ L"./shaders/closest_hit.hlsl", {
		L"closest_hit_shader"
	} });
	shaders_map.insert({ L"./shaders/miss_shaders.hlsl", miss_shaders });
	
	mPipeline = std::make_shared<raytracing_pipeline>();

	std::vector<shader_association> associations;
	std::vector<hit_group> groups;

	for (const auto& entity_cache : mResourceCache->entities_cache_data()) {
		associations.push_back(entity_cache.association);
		groups.push_back(entity_cache.group);
	}

	for (const auto& shaders : shaders_map) {
		for (const auto shader : shaders.second) {
			associations.push_back(shader_association(nullptr, config, shader));
		}
	}
	
	std::vector<std::shared_ptr<shader_library>> libraries;

	for (const auto& library : shaders_map) {
		libraries.push_back(std::make_shared<shader_library>(library.second,
			shader_library::compile_from_file(library.first)));
	}
	
	mPipeline->set_shader_libraries(libraries);
	mPipeline->set_shader_associations(associations);
	mPipeline->set_hit_group_shaders(groups);
	mPipeline->set_miss_shaders(miss_shaders);
	mPipeline->set_root_signature(mResourceScene->signature());
	mPipeline->set_ray_generation_shader(ray_generation_shader);
	mPipeline->set_max_depth(mMaxDepth);

	mPipeline->build(mDevice);

	const auto shader_table = mPipeline->shader_table();

	shader_table->begin_mapping();

	for (const auto& entity_cache : mResourceCache->entities_cache_data()) {
		std::memcpy(shader_table->shader_record_address(entity_cache.group.name) +
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

void path_tracing::dx::utilities::scene_pipeline::render(const std::shared_ptr<graphics_command_list>& command_list) const
{
	(*command_list)->SetPipelineState1(mPipeline->get());

	const auto shader_table = mPipeline->shader_table();
	const auto base_address = shader_table->address();
	
	D3D12_DISPATCH_RAYS_DESC desc = {};

	desc.Width = static_cast<UINT>(mResourceScene->render_target_hdr()->width());
	desc.Height = static_cast<UINT>(mResourceScene->render_target_hdr()->height());
	desc.Depth = 1;
	desc.RayGenerationShaderRecord.StartAddress = base_address + shader_table->ray_generation_shader().address;
	desc.RayGenerationShaderRecord.SizeInBytes = shader_table->ray_generation_shader().size;
	desc.MissShaderTable.StartAddress = base_address + shader_table->miss_shaders().address;
	desc.MissShaderTable.StrideInBytes = shader_table->miss_shaders().size;
	desc.MissShaderTable.SizeInBytes = shader_table->miss_shaders().size * mPipeline->miss_shaders().size();
	desc.HitGroupTable.StartAddress = base_address + shader_table->hit_group_shaders().address;
	desc.HitGroupTable.StrideInBytes = shader_table->hit_group_shaders().size;
	desc.HitGroupTable.SizeInBytes = shader_table->hit_group_shaders().size * mPipeline->hit_group_shaders().size();

	(*command_list)->DispatchRays(&desc);
}
