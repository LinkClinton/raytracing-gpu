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
	const std::optional<raytracing_instance>& instance,
	const std::optional<material_gpu_buffer>& material,
	const std::optional<emitter_gpu_buffer>& emitter,
	const std::optional<hit_group>& group) :
	instance(instance), material(material), emitter(emitter), group(group)
{
}

path_tracing::dx::utilities::resource_cache::resource_cache(const std::shared_ptr<device>& device) :
	mDevice(device)
{
	mCommandAllocator = std::make_shared<command_allocator>(mDevice);
	mCommandList = std::make_shared<graphics_command_list>(mCommandAllocator, mDevice);

	(*mCommandList)->Reset(mCommandAllocator->get(), nullptr);
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

path_tracing::dx::utilities::shape_cache_data path_tracing::dx::utilities::resource_cache::cache(const std::shared_ptr<shape>& shape) const
{
	return mShapeCache.at(shape);
}

std::shared_ptr<path_tracing::dx::wrapper::buffer>& path_tracing::dx::utilities::resource_cache::cpu_buffer()
{
	mCpuBuffers.push_back(nullptr);

	return mCpuBuffers.back();
}
