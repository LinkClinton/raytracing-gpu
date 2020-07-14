#include "shader_table.hpp"

#include <algorithm>
#include <cassert>

path_tracing::dx::wrapper::shader_functions::shader_functions(const shader_functions& other) :
	shader_functions(other.functions)
{
}

path_tracing::dx::wrapper::shader_functions::shader_functions(shader_functions&& other) noexcept :
	shader_functions(std::move(other.functions))
{
}

path_tracing::dx::wrapper::shader_functions::shader_functions(const std::vector<std::wstring>& functions) :
	functions(functions)
{
	pointers = std::vector<LPCWSTR>(functions.size());

	for (size_t index = 0; index < functions.size(); index++)
		pointers[index] = functions[index].c_str();
}

path_tracing::dx::wrapper::shader_functions& path_tracing::dx::wrapper::shader_functions::operator=(const shader_functions& other)
{
	functions = other.functions;

	pointers = std::vector<LPCWSTR>(functions.size());

	for (size_t index = 0; index < functions.size(); index++)
		pointers[index] = functions[index].c_str();

	return *this;
}

path_tracing::dx::wrapper::shader_functions& path_tracing::dx::wrapper::shader_functions::operator=(shader_functions&& other) noexcept
{
	functions = std::move(other.functions);

	pointers = std::vector<LPCWSTR>(functions.size());

	for (size_t index = 0; index < functions.size(); index++)
		pointers[index] = functions[index].c_str();

	return *this;
}

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

	mRayGenerationShader.address = 0;
	
	// find the max_size of each shader type
	// for ray generation shader, if it has root signature we can find the size from mShaderRecords
	// otherwise, the size should be "shader_without_signature_size"
	if (mShaderRecords.find(ray_generation_shader) != mShaderRecords.end())
		mRayGenerationShader.size = mShaderRecords[ray_generation_shader].size;
	else
		mRayGenerationShader.size = shader_without_signature_size;
	
	for (const auto& shader : miss_shaders) {
		auto shader_size = mShaderRecords.find(shader) != mShaderRecords.end() ? 
			mShaderRecords[shader].size : shader_without_signature_size;

		mMissShaders.size = std::max(mMissShaders.size, shader_size);
	}

	for (const auto& shader : hit_group_shaders) {
		auto shader_size = mShaderRecords.find(shader) != mShaderRecords.end() ?
			mShaderRecords[shader].size : shader_without_signature_size;

		mHitGroupShaders.size = std::max(mHitGroupShaders.size, shader_size);
	}
	
	// if the ray_generation_shader is not in the mShaderRecords, means the shader does not have local root signature
	// so we need insert a new shader record with size = shader_without_signature_size and offset = 0
	if (mShaderRecords.find(ray_generation_shader) == mShaderRecords.end())
		mShaderRecords.insert({ ray_generation_shader, shader_record(0, shader_without_signature_size) });

	auto shader_record_offset = mShaderRecords[ray_generation_shader].address + mShaderRecords[ray_generation_shader].size;

	mMissShaders.address = shader_record_offset;
	
	for (const auto& shader : miss_shaders) {
		mShaderRecords[shader] = shader_record(shader_record_offset, mMissShaders.size);

		shader_record_offset = shader_record_offset + mMissShaders.size;
	}

	mHitGroupShaders.address = shader_record_offset;
	
	for (const auto& shader : hit_group_shaders) {
		mShaderRecords[shader] = shader_record(shader_record_offset, mHitGroupShaders.size);

		shader_record_offset = shader_record_offset + mHitGroupShaders.size;
	}

	mCpuBuffer = std::make_shared<buffer>(device, D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_FLAG_NONE, D3D12_HEAP_TYPE_UPLOAD, shader_record_offset);
	mGpuBuffer = std::make_shared<buffer>(device, D3D12_RESOURCE_STATE_GENERIC_READ,
		D3D12_RESOURCE_FLAG_NONE, D3D12_HEAP_TYPE_DEFAULT, shader_record_offset);
}

void path_tracing::dx::wrapper::shader_table::copy_shader_identifier_from(
	const ComPtr<ID3D12StateObjectProperties>& properties)
{
	begin_mapping();

	for (const auto& shader : mShaderRecords)
		std::memcpy(mBufferMapping + shader.second.address,
			properties->GetShaderIdentifier(shader.first.c_str()),
			D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

	end_mapping();
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

path_tracing::dx::wrapper::shader_record path_tracing::dx::wrapper::shader_table::ray_generation_shader() const noexcept
{
	return mRayGenerationShader;
}

path_tracing::dx::wrapper::shader_record path_tracing::dx::wrapper::shader_table::hit_group_shaders() const noexcept
{
	return mHitGroupShaders;
}

path_tracing::dx::wrapper::shader_record path_tracing::dx::wrapper::shader_table::miss_shaders() const noexcept
{
	return mMissShaders;
}
