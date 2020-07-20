#include "resource_cache.hpp"

path_tracing::dx::utilities::shape_cache_data::shape_cache_data(
	const std::shared_ptr<buffer>& positions,
	const std::shared_ptr<buffer>& normals,
	const std::shared_ptr<buffer>& indices,
	const std::shared_ptr<buffer>& uvs) :
	positions(positions), normals(normals), indices(indices), uvs(uvs)
{
	geometry = std::make_shared<raytracing_geometry>(positions, indices);
}

path_tracing::dx::utilities::entity_cache_data::entity_cache_data(
	const shader_association& association,
	const hit_group& group) :
	association(association), group(group)
{
}

path_tracing::dx::utilities::shape_type_data::shape_type_data(const shader_association& association, const hit_group& group) :
	association(association), group(group)
{
}

path_tracing::dx::utilities::resource_cache::resource_cache(const std::shared_ptr<device>& device) :
	mDevice(device)
{
	mCommandAllocator = std::make_shared<command_allocator>(mDevice);
	mCommandList = std::make_shared<graphics_command_list>(mCommandAllocator, mDevice);

	(*mCommandList)->Reset(mCommandAllocator->get(), nullptr);

	shape_type_data triangles_type;

	triangles_type.association.root_signature = std::make_shared<root_signature>();
	triangles_type.association.config = std::nullopt;
	triangles_type.group = hit_group(D3D12_HIT_GROUP_TYPE_TRIANGLES, L"", L"closest_hit_shader",
		L"", L"");

	triangles_type.association.root_signature->add_srv("positions", 0, 100);
	triangles_type.association.root_signature->add_srv("normals", 1, 100);
	triangles_type.association.root_signature->add_srv("indices", 2, 100);
	triangles_type.association.root_signature->add_srv("uvs", 3, 100);
	triangles_type.association.root_signature->add_constants("index", 4, 100, 1);

	triangles_type.association.root_signature->serialize(mDevice, true);
	
	mTypeCache.insert({ "triangles", triangles_type });
}

void path_tracing::dx::utilities::resource_cache::execute(
	const std::vector<std::shared_ptr<entity>>& entities, 
	const std::shared_ptr<command_queue>& queue)
{
	// first, cache the shape data
	for (const auto& entity : entities) {
		// when the entity has not shape or the shape was cached we will continue
		if (!entity->has_component<shape>() || mShapesIndex.find(entity->component<shape>()) != mShapesIndex.end()) continue;

		auto positions = entity->component<shape>()->positions();
		auto normals = entity->component<shape>()->normals();
		auto indices = entity->component<shape>()->indices();
		auto uvs = entity->component<shape>()->uvs();

		shape_cache_data data;

		// the shape may not have normals or uvs, so we will create a empty array to avoid empty buffer
		if (normals.empty()) normals = std::vector<vector3>(positions.size());
		if (uvs.empty()) uvs = std::vector<vector3>(positions.size());

		// copy the data to gpu buffer
		copy_data_to_gpu_buffer(mCommandList, mDevice, positions, cpu_buffer(), data.positions);
		copy_data_to_gpu_buffer(mCommandList, mDevice, normals, cpu_buffer(), data.normals);
		copy_data_to_gpu_buffer(mCommandList, mDevice, indices, cpu_buffer(), data.indices);
		copy_data_to_gpu_buffer(mCommandList, mDevice, uvs, cpu_buffer(), data.uvs);

		data.geometry = std::make_shared<raytracing_geometry>(data.positions, data.indices);
		
		data.geometry->build(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE, mCommandList, mDevice);

		mShapes.push_back(entity->component<shape>()->gpu_buffer());

		mShapesIndex.insert({ entity->component<shape>(), mShapesCache.size() });
		mShapesCache.push_back(data);
	}

	// second, cache the entity data
	for (const auto& entity : entities) {
		if (entity->has_component<material>()) 
			mMaterials.push_back(entity->component<material>()->gpu_buffer());

		if (entity->has_component<emitter>()) 
			mEmitters.push_back(entity->component<emitter>()->gpu_buffer(entity->transform(), entity->index()));

		if (entity->has_component<shape>()) {
			const auto type_data = mTypeCache.at("triangles");
			const auto shape_data = mShapesCache[entity->component<shape>()->index()];

			entity_cache_data data;
			
			data.group = type_data.group;
			data.group.name = L"entity" + std::to_wstring(mEntitiesCache.size());

			data.association = shader_association(type_data.association.root_signature, type_data.association.config, data.group.name);

			data.shader_table_data = std::vector<byte>(type_data.association.root_signature->size());

			// copy the shader table data for this hit group
			const std::vector<D3D12_GPU_VIRTUAL_ADDRESS> gpu_handles = {
				(*shape_data.positions)->GetGPUVirtualAddress(),
				(*shape_data.normals)->GetGPUVirtualAddress(),
				(*shape_data.indices)->GetGPUVirtualAddress(),
				(*shape_data.uvs)->GetGPUVirtualAddress(),
			};

			const std::vector<std::string> signature_name = {
				"positions", "normals", "indices", "uvs"
			};

			for (size_t index = 0; index < gpu_handles.size(); index++) {
				const auto offset = type_data.association.root_signature->base(signature_name[index]);

				std::memcpy(data.shader_table_data.data() + offset,
					&gpu_handles[index], sizeof(D3D12_GPU_VIRTUAL_ADDRESS));
			}

			const auto index = entity->index();

			std::memcpy(data.shader_table_data.data() + type_data.association.root_signature->base("index"),
				&index, sizeof(index));

			mEntitiesCache.push_back(data);
			mInstances.push_back(raytracing_instance(shape_data.geometry, entity->transform().matrix));
		}

		mEntities.push_back(entity->gpu_buffer());
	}
	
	(*mCommandList)->Close();
	
	queue->execute({ mCommandList });
	queue->wait();

	(*mCommandAllocator)->Reset();
	(*mCommandList)->Reset(mCommandAllocator->get(), nullptr);

	mCpuBuffers.clear();
}

const std::vector<path_tracing::dx::utilities::entity_cache_data>& path_tracing::dx::utilities::resource_cache::entities_cache_data() const noexcept
{
	return mEntitiesCache;
}

const std::vector<path_tracing::dx::utilities::shape_cache_data>& path_tracing::dx::utilities::resource_cache::shapes_cache_data() const noexcept
{
	return mShapesCache;
}

const std::vector<path_tracing::dx::wrapper::raytracing_instance>& path_tracing::dx::utilities::resource_cache::instances() const noexcept
{
	return mInstances;
}

const std::vector<path_tracing::core::scenes::entity_gpu_buffer>& path_tracing::dx::utilities::resource_cache::entities() const noexcept
{
	return mEntities;
}

const std::vector<path_tracing::core::materials::material_gpu_buffer>& path_tracing::dx::utilities::resource_cache::materials() const noexcept
{
	return mMaterials;
}

const std::vector<path_tracing::core::emitters::emitter_gpu_buffer>& path_tracing::dx::utilities::resource_cache::emitters() const noexcept
{
	return mEmitters;
}

const std::vector<path_tracing::core::shapes::shape_gpu_buffer>& path_tracing::dx::utilities::resource_cache::shapes() const noexcept
{
	return mShapes;
}

std::shared_ptr<path_tracing::dx::wrapper::buffer>& path_tracing::dx::utilities::resource_cache::cpu_buffer()
{
	mCpuBuffers.push_back(nullptr);

	return mCpuBuffers.back();
}
