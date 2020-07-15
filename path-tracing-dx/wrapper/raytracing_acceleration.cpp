#include "raytracing_acceleration.hpp"

path_tracing::dx::wrapper::raytracing_geometry::raytracing_geometry(
	const std::shared_ptr<buffer>& positions, const std::shared_ptr<buffer>& indices)
{
	mDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	mDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;

	mDesc.Triangles.Transform3x4 = 0;
	mDesc.Triangles.VertexBuffer.StartAddress = (*positions)->GetGPUVirtualAddress();
	mDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(vector3);
	mDesc.Triangles.VertexCount = static_cast<UINT>(positions->size() / sizeof(vector3));
	mDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
	mDesc.Triangles.IndexBuffer = (*indices)->GetGPUVirtualAddress();
	mDesc.Triangles.IndexCount = static_cast<UINT>(indices->size() / sizeof(unsigned));
	mDesc.Triangles.IndexFormat = DXGI_FORMAT_R32_UINT;
}

void path_tracing::dx::wrapper::raytracing_geometry::build(
	const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS& flags,
	const std::shared_ptr<graphics_command_list>& command_list, 
	const std::shared_ptr<device>& device)
{
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};

	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = 1;
	inputs.Flags = flags;
	inputs.pGeometryDescs = &mDesc;

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO pre_build_info = {};

	(*device)->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &pre_build_info);

	pre_build_info.ResultDataMaxSizeInBytes = align_to(pre_build_info.ResultDataMaxSizeInBytes,
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
	pre_build_info.ScratchDataSizeInBytes = align_to(pre_build_info.ScratchDataSizeInBytes,
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);

	mAcceleration = std::make_shared<buffer>(device, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_HEAP_TYPE_DEFAULT, pre_build_info.ResultDataMaxSizeInBytes);

	mScratch = std::make_shared<buffer>(device, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_HEAP_TYPE_DEFAULT, pre_build_info.ScratchDataSizeInBytes);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};

	desc.Inputs = inputs;
	desc.DestAccelerationStructureData = (*mAcceleration)->GetGPUVirtualAddress();
	desc.ScratchAccelerationStructureData = (*mScratch)->GetGPUVirtualAddress();

	D3D12_RESOURCE_BARRIER barrier;

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = mAcceleration->get();

	(*command_list)->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
	(*command_list)->ResourceBarrier(1, &barrier);
}

std::shared_ptr<path_tracing::dx::wrapper::buffer> path_tracing::dx::wrapper::raytracing_geometry::acceleration() const noexcept
{
	return mAcceleration;
}

path_tracing::dx::wrapper::raytracing_instance::raytracing_instance(const std::shared_ptr<raytracing_geometry>& geometry, const matrix4x4& transform) :
	mGeometry(geometry), mTransform(transform)
{
}

std::shared_ptr<path_tracing::dx::wrapper::raytracing_geometry> path_tracing::dx::wrapper::raytracing_instance::geometry() const noexcept
{
	return mGeometry;
}

path_tracing::core::shared::matrix4x4 path_tracing::dx::wrapper::raytracing_instance::transform() const noexcept
{
	return mTransform;
}

path_tracing::dx::wrapper::raytracing_acceleration::raytracing_acceleration(const std::vector<std::shared_ptr<raytracing_instance>>& instances)
{
	mInstancesDesc = std::vector<D3D12_RAYTRACING_INSTANCE_DESC>(instances.size());

	for (size_t index = 0; index < instances.size(); index++) {
		auto& desc = mInstancesDesc[index];

		desc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_FORCE_OPAQUE;
		desc.InstanceID = static_cast<UINT>(index);
		desc.InstanceContributionToHitGroupIndex = static_cast<UINT>(index);
		desc.AccelerationStructure = (*instances[index]->geometry()->acceleration())->GetGPUVirtualAddress();
		desc.InstanceMask = 0xff;

		// convert column matrix to row matrix
		const auto matrix = transpose(instances[index]->transform());

		for (int r = 0; r < 3; r++) for (int c = 0; c < 4; c++) desc.Transform[r][c] = matrix[r][c];
	}
}

void path_tracing::dx::wrapper::raytracing_acceleration::build(
	const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS& flags,
	const std::shared_ptr<graphics_command_list>& command_list,
	const std::shared_ptr<device>& device)
{
	copy_data_to_gpu_buffer(command_list, device, mInstancesDesc, mCpuInstance, mGpuInstance);
	
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS inputs = {};

	inputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	inputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	inputs.NumDescs = static_cast<UINT>(mInstancesDesc.size());
	inputs.Flags = flags;
	inputs.InstanceDescs = (*mGpuInstance)->GetGPUVirtualAddress();

	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO pre_build_info = {};

	(*device)->GetRaytracingAccelerationStructurePrebuildInfo(&inputs, &pre_build_info);

	pre_build_info.ResultDataMaxSizeInBytes = align_to(pre_build_info.ResultDataMaxSizeInBytes,
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);
	pre_build_info.ScratchDataSizeInBytes = align_to(pre_build_info.ScratchDataSizeInBytes,
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT);

	mAcceleration = std::make_shared<buffer>(device, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_HEAP_TYPE_DEFAULT, pre_build_info.ResultDataMaxSizeInBytes);

	mScratch = std::make_shared<buffer>(device, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, D3D12_HEAP_TYPE_DEFAULT, pre_build_info.ScratchDataSizeInBytes);

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC desc = {};

	desc.Inputs = inputs;
	desc.DestAccelerationStructureData = (*mAcceleration)->GetGPUVirtualAddress();
	desc.ScratchAccelerationStructureData = (*mScratch)->GetGPUVirtualAddress();

	D3D12_RESOURCE_BARRIER barrier;

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.UAV.pResource = mAcceleration->get();

	(*command_list)->BuildRaytracingAccelerationStructure(&desc, 0, nullptr);
	(*command_list)->ResourceBarrier(1, &barrier);
}
