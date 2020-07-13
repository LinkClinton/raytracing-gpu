#include "descriptor_heap.hpp"

#include <cassert>

void path_tracing::dx::wrapper::descriptor_table::add_srv_range(const std::vector<std::string>& name, size_t base, size_t space)
{
	add_range(name, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, base, space);
}

void path_tracing::dx::wrapper::descriptor_table::add_uav_range(const std::vector<std::string>& name, size_t base, size_t space)
{
	add_range(name, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, base, space);
}

void path_tracing::dx::wrapper::descriptor_table::add_cbv_range(const std::vector<std::string>& name, size_t base, size_t space)
{
	add_range(name, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, base, space);
}

void path_tracing::dx::wrapper::descriptor_table::add_range(
	const std::vector<std::string>& name, const D3D12_DESCRIPTOR_RANGE_TYPE& type, size_t base, size_t space)
{
	D3D12_DESCRIPTOR_RANGE range = {};

	range.OffsetInDescriptorsFromTableStart = static_cast<UINT>(mDescriptorIndex.size());
	range.BaseShaderRegister = static_cast<UINT>(base);
	range.NumDescriptors = static_cast<UINT>(name.size());
	range.RegisterSpace = static_cast<UINT>(space);
	range.RangeType = type;

	for (size_t index = 0; index < name.size(); index++)
		mDescriptorIndex.insert({ name[index], mDescriptorIndex.size() });

	mRanges.push_back(range);
}

D3D12_ROOT_PARAMETER path_tracing::dx::wrapper::descriptor_table::root_parameter() const noexcept
{
	D3D12_ROOT_PARAMETER parameter = {};

	parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(mRanges.size());
	parameter.DescriptorTable.pDescriptorRanges = mRanges.data();

	return parameter;
}

path_tracing::dx::wrapper::descriptor_table path_tracing::dx::wrapper::descriptor_table::create()
{
	return descriptor_table();
}

size_t path_tracing::dx::wrapper::descriptor_table::descriptors() const noexcept
{
	return mDescriptorIndex.size();
}

size_t path_tracing::dx::wrapper::descriptor_table::operator[](const std::string& name) const
{
	return mDescriptorIndex.at(name);
}

ID3D12DescriptorHeap* const* path_tracing::dx::wrapper::descriptor_heap::get_address_of() const
{
	return mHeap.GetAddressOf();
}

ID3D12DescriptorHeap* path_tracing::dx::wrapper::descriptor_heap::operator->() const
{
	return mHeap.Get();
}

ID3D12DescriptorHeap* path_tracing::dx::wrapper::descriptor_heap::get() const
{
	return mHeap.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE path_tracing::dx::wrapper::descriptor_heap::cpu_handle(size_t index) const
{
	assert(index < mCount);

	return offset_handle(mHeap->GetCPUDescriptorHandleForHeapStart(), index * mOffset);
}

D3D12_GPU_DESCRIPTOR_HANDLE path_tracing::dx::wrapper::descriptor_heap::gpu_handle(size_t index) const
{
	assert(index < mCount);

	return offset_handle(mHeap->GetGPUDescriptorHandleForHeapStart(), index * mOffset);
}

size_t path_tracing::dx::wrapper::descriptor_heap::count() const noexcept
{
	return mCount;
}

path_tracing::dx::wrapper::descriptor_heap path_tracing::dx::wrapper::descriptor_heap::create(const device& device,
	const D3D12_DESCRIPTOR_HEAP_TYPE& type, size_t count)
{
	descriptor_heap heap;

	heap.mCount = count;
	heap.mOffset = device->GetDescriptorHandleIncrementSize(type);

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	desc.Type = type;
	desc.Flags = type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;
	desc.NumDescriptors = static_cast<UINT>(heap.mCount);

	device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap.mHeap.GetAddressOf()));

	return heap;
}

path_tracing::dx::wrapper::descriptor_heap path_tracing::dx::wrapper::descriptor_heap::create(const device& device, const descriptor_table& table)
{
	return create(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, table.descriptors());
}