#include "shader_library.hpp"

path_tracing::dx::wrapper::shader_association::shader_association(const std::vector<std::wstring>& functions) :
	functions(functions)
{
	pointers = std::vector<LPCWSTR>(functions.size());

	for (size_t index = 0; index < pointers.size(); index++)
		pointers[index] = functions[index].c_str();
}

path_tracing::dx::wrapper::shader_association::shader_association(const shader_association& other) :
	shader_association(other.functions)
{
}

path_tracing::dx::wrapper::shader_association::shader_association(shader_association&& other) noexcept
{
	functions = std::move(other.functions);
	pointers = std::move(other.pointers);
}

path_tracing::dx::wrapper::shader_association& path_tracing::dx::wrapper::shader_association::operator=(const shader_association& other)
{
	functions = other.functions;
	pointers = std::vector<LPCWSTR>(functions.size());

	for (size_t index = 0; index < pointers.size(); index++)
		pointers[index] = functions[index].c_str();

	return *this;
}

path_tracing::dx::wrapper::shader_association& path_tracing::dx::wrapper::shader_association::operator=(shader_association&& other) noexcept
{
	functions = std::move(other.functions);
	pointers = std::move(other.pointers);

	return *this;
}

path_tracing::dx::wrapper::shader_payload_config::shader_payload_config(const std::vector<std::wstring>& functions,
                                                                        size_t max_attribute_size, size_t max_payload_size) :
	shader_association(functions),
	config({ static_cast<UINT>(max_payload_size), static_cast<UINT>(max_attribute_size) })
{
}

path_tracing::dx::wrapper::shader_root_signature::shader_root_signature(const std::vector<std::wstring>& functions,
	const wrapper::root_signature& root_signature) :
	shader_association(functions), root_signature(root_signature)
{
}

path_tracing::dx::wrapper::shader_library::shader_library(const shader_library& other)
{
	mFunctions = other.mFunctions;
	mByteCode = other.mByteCode;

	mExports = std::vector<D3D12_EXPORT_DESC>(mFunctions.size());


	internal_build_exports();
}

path_tracing::dx::wrapper::shader_library::shader_library(shader_library&& other) noexcept
{
	mFunctions = std::move(other.mFunctions);
	mByteCode = std::move(other.mByteCode);

	mExports = std::move(other.mExports);

	internal_build_exports();
}

path_tracing::dx::wrapper::shader_library& path_tracing::dx::wrapper::shader_library::operator=(const shader_library& other)
{
	mFunctions = other.mFunctions;
	mByteCode = other.mByteCode;

	mExports = std::vector<D3D12_EXPORT_DESC>(mFunctions.size());

	internal_build_exports();

	return *this;
}

path_tracing::dx::wrapper::shader_library& path_tracing::dx::wrapper::shader_library::operator=(shader_library&& other) noexcept
{
	mFunctions = std::move(other.mFunctions);
	mByteCode = std::move(other.mByteCode);

	mExports = std::move(other.mExports);

	internal_build_exports();

	return *this;
}

D3D12_DXIL_LIBRARY_DESC path_tracing::dx::wrapper::shader_library::desc() const noexcept
{
	return mDesc;
}

path_tracing::dx::wrapper::shader_library path_tracing::dx::wrapper::shader_library::create(const std::vector<std::wstring>& functions,
	const std::vector<byte>& byte_code)
{
	shader_library library;

	library.mByteCode = byte_code;
	library.mFunctions = functions;

	library.mExports = std::vector<D3D12_EXPORT_DESC>(library.mFunctions.size());

	library.internal_build_exports();
	
	return library;
}

void path_tracing::dx::wrapper::shader_library::internal_build_exports()
{
	for (size_t index = 0; index < mFunctions.size(); index++) {
		mExports[index].Name = mFunctions[index].c_str();
		mExports[index].Flags = D3D12_EXPORT_FLAG_NONE;
		mExports[index].ExportToRename = nullptr;
	}

	mDesc.pExports = mExports.data();
	mDesc.NumExports = static_cast<UINT>(mExports.size());
	mDesc.DXILLibrary.BytecodeLength = static_cast<UINT>(mByteCode.size());
	mDesc.DXILLibrary.pShaderBytecode = mByteCode.data();
}
