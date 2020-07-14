#include "shader_library.hpp"

#include <cassert>

path_tracing::dx::wrapper::shader_raytracing_config::shader_raytracing_config(size_t max_attribute_size, size_t max_payload_size) :
	max_attribute_size(max_attribute_size), max_payload_size(max_payload_size)
{
	
}

size_t path_tracing::dx::wrapper::shader_raytracing_config::hash() const noexcept
{
	assert(max_attribute_size <= D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES);
	
	return max_payload_size * D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES + max_attribute_size;
}

path_tracing::dx::wrapper::shader_association::shader_association(
	const std::shared_ptr<wrapper::root_signature>& root_signature,
	const shader_raytracing_config& config, 
	const std::wstring& name) :
	root_signature(root_signature), config(config), name(name)
{
}

path_tracing::dx::wrapper::shader_library::shader_library(const std::vector<std::wstring>& functions,
                                                          const std::vector<byte>& code) :
	mFunctions(functions), mByteCode(code)
{
	mExports = std::vector<D3D12_EXPORT_DESC>(functions.size());
	
	for (size_t index = 0; index < mFunctions.size(); index++) {
		mExports[index].Flags = D3D12_EXPORT_FLAG_NONE;
		mExports[index].ExportToRename = nullptr;
		mExports[index].Name = functions[index].c_str();
	}

	mDesc.pExports = mExports.data();
	mDesc.DXILLibrary.pShaderBytecode = mByteCode.data();
	mDesc.DXILLibrary.BytecodeLength = static_cast<UINT>(mByteCode.size());
	mDesc.NumExports = static_cast<UINT>(mExports.size());
}

D3D12_DXIL_LIBRARY_DESC path_tracing::dx::wrapper::shader_library::desc() const
{
	return mDesc;
}
