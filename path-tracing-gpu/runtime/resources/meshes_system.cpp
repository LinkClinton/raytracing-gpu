#include "meshes_system.hpp"

namespace path_tracing::runtime::resources {

	template <typename T>
	void copy_all_to(const std::vector<T>& data, void* address)
	{
		std::memcpy(address, data.data(), sizeof(T) * data.size());
	}
	
}

void path_tracing::runtime::resources::meshes_system::upload_cached_buffers(const render_device& device)
{
	size_t vtx_extend_count = 0;
	size_t idx_extend_count = 0;

	for (const auto& mesh : mCachedMeshes) {
		assert(!mesh.second.positions.empty() && !mesh.second.indices.empty());
		
		vtx_extend_count += mesh.second.positions.size();
		idx_extend_count += mesh.second.indices.size();
	}

	mesh_cpu_buffer new_cpu_buffer;

	new_cpu_buffer.positions = std::vector<vector3>(mCpuBuffers.positions.size() + vtx_extend_count);
	new_cpu_buffer.normals = std::vector<vector3>(mCpuBuffers.normals.size() + vtx_extend_count);
	new_cpu_buffer.uvs = std::vector<vector3>(mCpuBuffers.uvs.size() + vtx_extend_count);

	new_cpu_buffer.indices = std::vector<uint32>(mCpuBuffers.indices.size() + idx_extend_count);

	copy_all_to(mCpuBuffers.positions, new_cpu_buffer.positions.data());
	copy_all_to(mCpuBuffers.normals, new_cpu_buffer.normals.data());
	copy_all_to(mCpuBuffers.uvs, new_cpu_buffer.uvs.data());

	copy_all_to(mCpuBuffers.indices, new_cpu_buffer.indices.data());
	
	size_t vtx_base_count = mCpuBuffers.positions.size();
	size_t idx_base_count = mCpuBuffers.indices.size();

	for (const auto& mesh : mCachedMeshes) {
		const auto mesh_vtx_count = mesh.second.positions.size();
		const auto mesh_idx_count = mesh.second.indices.size();
		
		copy_all_to(mesh.second.positions, new_cpu_buffer.positions.data() + vtx_base_count);
		copy_all_to(mesh.second.normals, new_cpu_buffer.normals.data() + vtx_base_count);
		copy_all_to(mesh.second.uvs, new_cpu_buffer.uvs.data() + vtx_base_count);

		copy_all_to(mesh.second.indices, new_cpu_buffer.indices.data() + idx_base_count);
		
		vtx_base_count += mesh_vtx_count;
		idx_base_count += mesh_idx_count;
	}

	mCpuBuffers = std::move(new_cpu_buffer);
	
	if (!mCachedMeshes.empty()) allocate_gpu_buffer_memory(device);
}

path_tracing::runtime::resources::mesh_info path_tracing::runtime::resources::meshes_system::allocate(const std::string& name,
	const mesh_cpu_buffer& mesh)
{
	mMeshInfos.insert({ name, mesh_info{
		static_cast<uint32>(mVtxBaseCount), static_cast<uint32>(mesh.positions.size()),
		static_cast<uint32>(mIdxBaseCount), static_cast<uint32>(mesh.indices.size())
	} });

	mMeshProperties.insert({ static_cast<uint32>(mIdxBaseCount), { !mesh.normals.empty(), !mesh.uvs.empty() } });
	
	mVtxBaseCount += mesh.positions.size();
	mIdxBaseCount += mesh.indices.size();

	mCachedMeshes.push_back({ name, mesh });

	return mMeshInfos.at(name);
}

path_tracing::runtime::resources::mesh_info path_tracing::runtime::resources::meshes_system::allocate(const std::string& name,
	mesh_cpu_buffer&& mesh)
{
	mMeshInfos.insert({ name, mesh_info{
		static_cast<uint32>(mVtxBaseCount), static_cast<uint32>(mesh.positions.size()),
		static_cast<uint32>(mIdxBaseCount), static_cast<uint32>(mesh.indices.size())
	} });

	mMeshProperties.insert({ static_cast<uint32>(mIdxBaseCount), { !mesh.normals.empty(), !mesh.uvs.empty() } });

	mVtxBaseCount += mesh.positions.size();
	mIdxBaseCount += mesh.indices.size();

	mCachedMeshes.push_back({ name, mesh });

	return mMeshInfos.at(name);
}

path_tracing::runtime::resources::mesh_info path_tracing::runtime::resources::meshes_system::info(const std::string& name) const
{
	return mMeshInfos.at(name);
}

path_tracing::runtime::resources::mesh_property path_tracing::runtime::resources::meshes_system::property(
	const mesh_info& info) const
{
	return mMeshProperties.at(info.idx_location);
}

wrapper::directx12::raytracing_geometry path_tracing::runtime::resources::meshes_system::geometry(const mesh_info& info) const
{
	return mGeometries.at(info.idx_location);
}

const path_tracing::runtime::resources::mesh_cpu_buffer& path_tracing::runtime::resources::meshes_system::cpu_buffer() const noexcept
{
	return mCpuBuffers;
}

const path_tracing::runtime::resources::mesh_gpu_buffer& path_tracing::runtime::resources::meshes_system::gpu_buffer() const noexcept
{
	return mGpuBuffers;
}

bool path_tracing::runtime::resources::meshes_system::has(const std::string& name) const noexcept
{
	return mMeshInfos.find(name) != mMeshInfos.end();
}

bool path_tracing::runtime::resources::meshes_system::has(const mesh_info& info) const noexcept
{
	return mGeometries.find(info.idx_location) != mGeometries.end();
}

size_t path_tracing::runtime::resources::meshes_system::count() const noexcept
{
	return mMeshInfos.size();
}

void path_tracing::runtime::resources::meshes_system::allocate_gpu_buffer_memory(const render_device& device)
{
	if (mCommandAllocator.get() == nullptr) {
		mCommandAllocator = wrapper::directx12::command_allocator::create(device.device());
		mCommandList = wrapper::directx12::graphics_command_list::create(device.device(), mCommandAllocator);
	}
	
	const auto default_info = wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_COPY_DEST);
	const auto upload_info = wrapper::directx12::resource_info::upload();

	const auto vtx_buffer_size = sizeof(vector3) * mVtxBaseCount;
	const auto idx_buffer_size = sizeof(uint32) * mIdxBaseCount;

	mUploads.positions = wrapper::directx12::buffer::create(device.device(), upload_info, vtx_buffer_size);
	mUploads.normals = wrapper::directx12::buffer::create(device.device(), upload_info, vtx_buffer_size);
	mUploads.uvs = wrapper::directx12::buffer::create(device.device(), upload_info, vtx_buffer_size);
	mUploads.indices = wrapper::directx12::buffer::create(device.device(), upload_info, idx_buffer_size);

	mUploads.positions.copy_from_cpu(mCpuBuffers.positions.data(), vtx_buffer_size);
	mUploads.normals.copy_from_cpu(mCpuBuffers.normals.data(), vtx_buffer_size);
	mUploads.uvs.copy_from_cpu(mCpuBuffers.uvs.data(), vtx_buffer_size);
	mUploads.indices.copy_from_cpu(mCpuBuffers.indices.data(), idx_buffer_size);

	mGpuBuffers.positions = wrapper::directx12::buffer::create(device.device(), default_info, vtx_buffer_size);
	mGpuBuffers.normals = wrapper::directx12::buffer::create(device.device(), default_info, vtx_buffer_size);
	mGpuBuffers.uvs = wrapper::directx12::buffer::create(device.device(), default_info, vtx_buffer_size);
	mGpuBuffers.indices = wrapper::directx12::buffer::create(device.device(), default_info, idx_buffer_size);

	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.get(), nullptr);

	mGpuBuffers.positions.copy_from(mCommandList, mUploads.positions);
	mGpuBuffers.normals.copy_from(mCommandList, mUploads.normals);
	mGpuBuffers.uvs.copy_from(mCommandList, mUploads.uvs);
	mGpuBuffers.indices.copy_from(mCommandList, mUploads.indices);

	std::vector<D3D12_RESOURCE_BARRIER> barriers;

	barriers.push_back(mGpuBuffers.positions.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	barriers.push_back(mGpuBuffers.normals.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	barriers.push_back(mGpuBuffers.uvs.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	barriers.push_back(mGpuBuffers.indices.barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

	mCommandList.resource_barrier(barriers);

	mGeometries.clear();

	// rebuild raytracing geometries of meshes
	// because raytracing geometries will reference the mesh buffer
	// see https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_raytracing_geometry_triangles_desc
	// we need clear up all geometries and rebuild all
	for (const auto& iterator : mMeshInfos) {
		const auto& mesh_info = iterator.second;

		const auto vtx_address = mGpuBuffers.positions->GetGPUVirtualAddress() + 
			mesh_info.vtx_location * sizeof(vector3);
		const auto idx_address = mGpuBuffers.indices->GetGPUVirtualAddress() +
			mesh_info.idx_location * sizeof(uint32);
		
		const auto geometry = wrapper::directx12::raytracing_geometry::create(
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE,
			mCommandList, device.device(),
			vtx_address, mesh_info.vtx_count,
			idx_address, mesh_info.idx_count
		);

		// we use mesh_info::idx_location as key value
		// because there is no mesh_info has same idx_location
		mGeometries.insert({ mesh_info.idx_location, geometry });
	}
	
	mCommandList.close();

	device.queue().execute({ mCommandList });
}
