#include "shader_library.hpp"

#include <dxcapi.h>
#include <cassert>

#pragma comment(lib, "dxcompiler.lib")

path_tracing::dx::wrapper::shader_raytracing_config::shader_raytracing_config(size_t max_attribute_size, size_t max_payload_size) :
	max_attribute_size(max_attribute_size), max_payload_size(max_payload_size)
{
	
}

size_t path_tracing::dx::wrapper::shader_raytracing_config::hash() const noexcept
{
	assert(max_attribute_size <= D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES);

	return encode(*this);
}

size_t path_tracing::dx::wrapper::shader_raytracing_config::encode(const shader_raytracing_config& config)
{
	return config.max_payload_size * D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES + config.max_attribute_size;
}

path_tracing::dx::wrapper::shader_raytracing_config path_tracing::dx::wrapper::shader_raytracing_config::decode(size_t code)
{
	return shader_raytracing_config(
		code % D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES, 
		code / D3D12_RAYTRACING_MAX_ATTRIBUTE_SIZE_IN_BYTES);
}

path_tracing::dx::wrapper::shader_association::shader_association(
	const std::shared_ptr<wrapper::root_signature>& root_signature,
	const std::optional<shader_raytracing_config>& config, 
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

std::vector<byte> path_tracing::dx::wrapper::shader_library::compile_from_file(const std::wstring& file_name)
{
	ComPtr<IDxcIncludeHandler> include;
	ComPtr<IDxcCompiler> compiler;
	ComPtr<IDxcLibrary> library;
	
	DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(library.GetAddressOf()));
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf()));

	library->CreateIncludeHandler(include.GetAddressOf());

	ComPtr<IDxcBlobEncoding> encoding_blob;

	library->CreateBlobFromFile(file_name.c_str(), nullptr, encoding_blob.GetAddressOf());

	ComPtr<IDxcOperationResult> result;

	std::vector<DxcDefine> defines;

	defines.push_back({ L"__HLSL_SHADER__", L"1" });

	const wchar_t* arguments[] = {
		L"",
	};

#ifndef _DEBUG
	compiler->Compile(encoding_blob.Get(), file_name.c_str(), L"",
		L"lib_6_4", arguments, 1, defines.data(), static_cast<UINT32>(defines.size()),
		include.Get(), result.GetAddressOf());
#else
	compiler->Compile(encoding_blob.Get(), file_name.c_str(), L"",
		L"lib_6_4", nullptr, 0, defines.data(), static_cast<UINT32>(defines.size()),
		include.Get(), result.GetAddressOf());
#endif
	
	ComPtr<IDxcBlobEncoding> error_code;
	ComPtr<IDxcBlob> result_code;
	HRESULT result_status;

	result->GetErrorBuffer(error_code.GetAddressOf());
	result->GetResult(result_code.GetAddressOf());
	result->GetStatus(&result_status);
	
	std::string error;

	error.resize(error_code->GetBufferSize());

	std::memcpy(error.data(), error_code->GetBufferPointer(), error_code->GetBufferSize());

	if (!error.empty() && error.back() == '\n') error.pop_back();
	if (!error.empty()) printf("%s\n", error.c_str());
	
	auto code = std::vector<byte>(result_code->GetBufferSize());

	std::memcpy(code.data(), result_code->GetBufferPointer(), result_code->GetBufferSize());

	return code;
}
