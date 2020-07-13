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

void path_tracing::dx::wrapper::descriptor_table::add_range(const std::vector<std::string>& name,
                                                            const D3D12_DESCRIPTOR_RANGE_TYPE& type, size_t base, size_t space)
{
	D3D12_DESCRIPTOR_RANGE range;

	range.OffsetInDescriptorsFromTableStart = static_cast<UINT>(mDescriptorsIndex.size());
	range.BaseShaderRegister = static_cast<UINT>(base);
	range.NumDescriptors = static_cast<UINT>(name.size());
	range.RegisterSpace = static_cast<UINT>(space);
	range.RangeType = type;

	for (size_t index = 0; index < name.size(); index++)
		mDescriptorsIndex.insert({ name[index], mDescriptorsIndex.size() });

	mRanges.push_back(range);
}

D3D12_ROOT_PARAMETER path_tracing::dx::wrapper::descriptor_table::parameter() const
{
	D3D12_ROOT_PARAMETER parameter;

	parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	parameter.DescriptorTable.pDescriptorRanges = mRanges.data();
	parameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(mRanges.size());

	return parameter;
}

size_t path_tracing::dx::wrapper::descriptor_table::index(const std::string& name) const
{
	return mDescriptorsIndex.at(name);
}

size_t path_tracing::dx::wrapper::descriptor_table::count() const noexcept
{
	return mDescriptorsIndex.size();
}

path_tracing::dx::wrapper::descriptor_heap::descriptor_heap(
	const std::shared_ptr<device>& device, const D3D12_DESCRIPTOR_HEAP_TYPE& type, size_t count) :
	mCount(count)
{
	mOffset = (*device)->GetDescriptorHandleIncrementSize(type);

	D3D12_DESCRIPTOR_HEAP_DESC desc;

	desc.Type = type;
	desc.Flags = type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;
	desc.NumDescriptors = static_cast<UINT>(mCount);

	(*device)->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mHeap.GetAddressOf()));
}

path_tracing::dx::wrapper::descriptor_heap::descriptor_heap(const std::shared_ptr<device>& device, const std::shared_ptr<descriptor_table>& table) :
	mCount(table->count())
{
	mOffset = (*device)->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_DESCRIPTOR_HEAP_DESC desc;

	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	desc.NumDescriptors = static_cast<UINT>(mCount);

	(*device)->CreateDescriptorHeap(&desc, IID_PPV_ARGS(mHeap.GetAddressOf()));
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