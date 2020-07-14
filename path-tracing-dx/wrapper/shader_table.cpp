#include "shader_table.hpp"

#include <algorithm>
#include <cassert>

path_tracing::dx::wrapper::shader_record::shader_record(size_t address, size_t size) :
	address(address), size(size)
{
}

path_tracing::dx::wrapper::shader_table::shader_table(
	const std::shared_ptr<device>& device,
	const std::vector<shader_association>& associations,
	const std::vector<std::wstring>& hit_group_shaders,
	const std::vector<std::wstring>& miss_shaders, 
	const std::wstring& ray_generation_shader)
{
	// first, build the shader record.size with shader has root signature
	// the size of shader_record is not the real size
	for (const auto& association : associations) {
		const auto size = align_to(
			D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES + association.root_signature->size(),
			D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
		
		mShaderRecords.insert({ association.name, shader_record(0, size) });
	}

	const auto shader_without_signature_size = align_to(D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES, 
		D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);

	// find the max_size of each shader type
	// for ray generation shader, if it has root signature we can find the size from mShaderRecords
	// otherwise, the size should be "shader_without_signature_size"
	if (mShaderRecords.find(ray_generation_shader) != mShaderRecords.end())
		mRayGenerationShaderSize = mShaderRecords[ray_generation_shader].size;
	else
		mRayGenerationShaderSize = shader_without_signature_size;

	for (const auto& shader : miss_shaders) {
		auto shader_size = mShaderRecords.find(shader) != mShaderRecords.end() ? 
			mShaderRecords[shader].size : shader_without_signature_size;

		mMissShaderSize = std::max(mMissShaderSize, shader_size);
	}

	for (const auto& shader : hit_group_shaders) {
		auto shader_size = mShaderRecords.find(shader) != mShaderRecords.end() ?
			mShaderRecords[shader].size : shader_without_signature_size;

		mHitGroupShaderSize = std::max(mHitGroupShaderSize, shader_size);
	}
	
	// if the ray_generation_shader is not in the mShaderRecords, means the shader does not have local root signature
	// so we need insert a new shader record with size = shader_without_signature_size and offset = 0
	if (mShaderRecords.find(ray_generation_shader) == mShaderRecords.end())
		mShaderRecords.insert({ ray_generation_shader, shader_record(0, shader_without_signature_size) });

	auto shader_record_offset = mShaderRecords[ray_generation_shader].address + mShaderRecords[ray_generation_shader].size;

	for (const auto& shader : miss_shaders) {
		mShaderRecords[shader] = shader_record(shader_record_offset, mMissShaderSize);

		shader_record_offset = shader_record_offset + mMissShaderSize;
	}

	for (const auto& shader : hit_group_shaders) {
		mShaderRecords[shader] = shader_record(shader_record_offset, mHitGroupShaderSize);

		shader_record_offset = shader_record_offset + mHitGroupShaderSize;
	}

	mCpuBuffer = std::make_shared<buffer>(device, D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_FLAG_NONE, D3D12_HEAP_TYPE_UPLOAD, shader_record_offset);
	mGpuBuffer = std::make_shared<buffer>(device, D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_FLAG_NONE, D3D12_HEAP_TYPE_DEFAULT, shader_record_offset);
}

void path_tracing::dx::wrapper::shader_table::begin_mapping()
{
	mBufferMapping = static_cast<byte*>(mCpuBuffer->map());
}

void path_tracing::dx::wrapper::shader_table::end_mapping()
{
	mBufferMapping = nullptr;
	mCpuBuffer->unmap();
}

void path_tracing::dx::wrapper::shader_table::upload(const std::shared_ptr<graphics_command_list>& command_list) const
{
	assert(mBufferMapping == nullptr);

	const auto before_barrier = mGpuBuffer->barrier(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST);
	const auto after_barrier = mGpuBuffer->barrier(D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);

	(*command_list)->ResourceBarrier(1, &before_barrier);
	(*command_list)->CopyResource(mGpuBuffer->get(), mCpuBuffer->get());
	(*command_list)->ResourceBarrier(1, &after_barrier);
}

byte* path_tracing::dx::wrapper::shader_table::shader_record_address(const std::wstring& name) const
{
	assert(mBufferMapping != nullptr);

	return mBufferMapping + mShaderRecords.at(name).address;
}

size_t path_tracing::dx::wrapper::shader_table::ray_generation_shader_size() const noexcept
{
	return mRayGenerationShaderSize;
}

size_t path_tracing::dx::wrapper::shader_table::hit_group_shader_size() const noexcept
{
	return mHitGroupShaderSize;
}

size_t path_tracing::dx::wrapper::shader_table::miss_shader_size() const noexcept
{
	return mMissShaderSize;
}
