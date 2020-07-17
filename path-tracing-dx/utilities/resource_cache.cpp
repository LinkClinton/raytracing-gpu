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
	const std::optional<shader_association>& association,
	const std::optional<raytracing_instance>& instance,
	const std::optional<material_gpu_buffer>& material,
	const std::optional<emitter_gpu_buffer>& emitter,
	const std::optional<hit_group>& group) :
	association(association), instance(instance), material(material), emitter(emitter), group(group)
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

	triangles_type.association.root_signature->serialize(mDevice, true);
	
	mTypeCache.insert({ "triangles", triangles_type });
}

void path_tracing::dx::utilities::resource_cache::cache_entity(const std::shared_ptr<entity>& entity)
{
	entity_cache_data data;

	if (entity->has_component<emitter>())
		data.emitter = entity->component<emitter>()->gpu_buffer(entity->transform());

	// if the entity has shape, it should has material too
	if (entity->has_component<material>() && entity->has_component<shape>()) {
		
		if (!has_cache(entity->component<shape>())) cache_shape(entity->component<shape>());
		
		// get type data of this type of entity(triangles, sphere and so on)
		const auto type_data = mTypeCache.at("triangles");
		const auto shape_data = mShapesCache.at(entity->component<shape>());
		
		data.material = entity->component<material>()->gpu_buffer();

		data.group = type_data.group;
		data.group->name = L"entity" + std::to_wstring(mEntitiesCache.size());
		
		data.instance = raytracing_instance(shape_data.geometry, entity->transform().matrix);
		data.association = shader_association(type_data.association.root_signature, type_data.association.config, data.group->name);

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
	}

	mEntitiesCache.insert({ entity, data });
}

void path_tracing::dx::utilities::resource_cache::cache_shape(const std::shared_ptr<shape>& shape)
{
	const auto mesh_data = shape->mesh();
	
	shape_cache_data data;

	copy_data_to_gpu_buffer(mCommandList, mDevice, mesh_data.positions, cpu_buffer(), data.positions);
	copy_data_to_gpu_buffer(mCommandList, mDevice, mesh_data.indices, cpu_buffer(), data.indices);

	if (!mesh_data.normals.empty()) copy_data_to_gpu_buffer(mCommandList, mDevice, mesh_data.normals, cpu_buffer(), data.normals);
	if (!mesh_data.uvs.empty()) copy_data_to_gpu_buffer(mCommandList, mDevice, mesh_data.uvs, cpu_buffer(), data.uvs);

	data.geometry = std::make_shared<raytracing_geometry>(data.positions, data.indices);

	data.geometry->build(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE, mCommandList, mDevice);

	mShapesCache.insert({ shape, data });
}

void path_tracing::dx::utilities::resource_cache::execute(const std::shared_ptr<command_queue>& queue)
{
	(*mCommandList)->Close();
	
	queue->execute({ mCommandList });
	queue->wait();

	(*mCommandAllocator)->Reset();
	(*mCommandList)->Reset(mCommandAllocator->get(), nullptr);

	mCpuBuffers.clear();
}

path_tracing::dx::utilities::entity_cache_data path_tracing::dx::utilities::resource_cache::cache(const std::shared_ptr<entity>& entity) const
{
	return mEntitiesCache.at(entity);
}

path_tracing::dx::utilities::shape_cache_data path_tracing::dx::utilities::resource_cache::cache(const std::shared_ptr<shape>& shape) const
{
	return mShapesCache.at(shape);
}

bool path_tracing::dx::utilities::resource_cache::has_cache(const std::shared_ptr<entity>& entity) const noexcept
{
	if (mEntitiesCache.find(entity) == mEntitiesCache.end()) return false;

	return true;
}

bool path_tracing::dx::utilities::resource_cache::has_cache(const std::shared_ptr<shape>& shape) const noexcept
{
	if (mShapesCache.find(shape) == mShapesCache.end()) return false;

	return true;
}

std::shared_ptr<path_tracing::dx::wrapper::buffer>& path_tracing::dx::utilities::resource_cache::cpu_buffer()
{
	mCpuBuffers.push_back(nullptr);

	return mCpuBuffers.back();
}
