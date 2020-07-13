#include "root_signature.hpp"

ID3D12RootSignature* const* path_tracing::dx::wrapper::root_signature::get_address_of() const
{
	return mRootSignature.GetAddressOf();
}

ID3D12RootSignature* path_tracing::dx::wrapper::root_signature::operator->() const
{
	return mRootSignature.Get();
}

ID3D12RootSignature* path_tracing::dx::wrapper::root_signature::get() const
{
	return mRootSignature.Get();
}

void path_tracing::dx::wrapper::root_signature::add_descriptor(const std::string& name,
	const D3D12_ROOT_PARAMETER_TYPE& type, size_t base, size_t space)
{
	D3D12_ROOT_PARAMETER parameter;

	parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	parameter.ParameterType = type;
	parameter.Descriptor.ShaderRegister = static_cast<UINT>(base);
	parameter.Descriptor.RegisterSpace = static_cast<UINT>(space);

	add_root_parameter(name, parameter);
}

void path_tracing::dx::wrapper::root_signature::add_descriptor_table(const std::string& name,
	const std::shared_ptr<descriptor_table>& table)
{
	add_root_parameter(name, table->parameter());
}

void path_tracing::dx::wrapper::root_signature::add_constants(const std::string& name, size_t base, size_t space, size_t count)
{
	D3D12_ROOT_PARAMETER parameter;

	parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	parameter.Constants.Num32BitValues = static_cast<UINT>(count);
	parameter.Constants.ShaderRegister = static_cast<UINT>(base);
	parameter.Constants.RegisterSpace = static_cast<UINT>(space);

	add_root_parameter(name, parameter);
}

void path_tracing::dx::wrapper::root_signature::add_srv(const std::string& name, size_t base, size_t space)
{
	add_descriptor(name, D3D12_ROOT_PARAMETER_TYPE_SRV, base, space);
}

void path_tracing::dx::wrapper::root_signature::add_uav(const std::string& name, size_t base, size_t space)
{
	add_descriptor(name, D3D12_ROOT_PARAMETER_TYPE_UAV, base, space);
}

void path_tracing::dx::wrapper::root_signature::add_cbv(const std::string& name, size_t base, size_t space)
{
	add_descriptor(name, D3D12_ROOT_PARAMETER_TYPE_CBV, base, space);
}

void path_tracing::dx::wrapper::root_signature::serialize(const std::shared_ptr<device>& device)
{
	D3D12_ROOT_SIGNATURE_DESC desc = {};

	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	desc.pParameters = mRootParameters.data();
	desc.NumParameters = static_cast<UINT>(mRootParameters.size());

	ComPtr<ID3DBlob> signature_blob = nullptr;
	ComPtr<ID3DBlob> error_blob = nullptr;

	D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, signature_blob.GetAddressOf(), error_blob.GetAddressOf());

	(*device)->CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(),
		IID_PPV_ARGS(mRootSignature.GetAddressOf()));
}

size_t path_tracing::dx::wrapper::root_signature::base(const std::string& name) const
{
	return mDescriptorBase[mDescriptorIndex.at(name)];
}

size_t path_tracing::dx::wrapper::root_signature::size(const std::string& name) const
{
	return mDescriptorSize[mDescriptorIndex.at(name)];
}


void path_tracing::dx::wrapper::root_signature::add_root_parameter(const std::string& name, const D3D12_ROOT_PARAMETER& parameter)
{
	mRootParameters.push_back(parameter);

	mDescriptorIndex.insert({ name, mDescriptorIndex.size() });

	const auto base = mDescriptorIndex.empty() ? D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES : mDescriptorBase.back();
	const auto size = parameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS ?
		align_to(static_cast<size_t>(parameter.Constants.Num32BitValues) * 4, sizeof(D3D12_GPU_DESCRIPTOR_HANDLE)) :
		sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);

	mDescriptorBase.push_back(base);
	mDescriptorSize.push_back(size);
}
