#include "shader_resource.hpp"

ID3D12Resource* const* path_tracing::dx::wrapper::shader_resource::get_address_of() const
{
	return mResource.GetAddressOf();
}

ID3D12Resource* path_tracing::dx::wrapper::shader_resource::operator->() const
{
	return mResource.Get();
}

ID3D12Resource* path_tracing::dx::wrapper::shader_resource::get() const
{
	return mResource.Get();
}

D3D12_RESOURCE_BARRIER path_tracing::dx::wrapper::shader_resource::barrier(
	const D3D12_RESOURCE_STATES& before,
	const D3D12_RESOURCE_STATES& after) const
{
	D3D12_RESOURCE_BARRIER barrier;

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.pResource = mResource.Get();

	return barrier;
}
path_tracing::dx::wrapper::buffer::buffer(
	const std::shared_ptr<device>& device, 
	const D3D12_RESOURCE_STATES& state,
	const D3D12_RESOURCE_FLAGS& flags,
	const D3D12_HEAP_TYPE& type, size_t size) :
	mSize(size)
{
	D3D12_RESOURCE_DESC desc;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = static_cast<UINT64>(mSize);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = flags;

	D3D12_HEAP_PROPERTIES properties = {
		type,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0,
	};

	(*device)->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE,
		&desc, state, nullptr, IID_PPV_ARGS(mResource.GetAddressOf()));
}

size_t path_tracing::dx::wrapper::buffer::size() const noexcept
{
	return mSize;
}

void* path_tracing::dx::wrapper::buffer::map() const
{
	void* data = nullptr;

	mResource->Map(0, nullptr, &data);

	return data;
}

void path_tracing::dx::wrapper::buffer::unmap() const
{
	mResource->Unmap(0, nullptr);
}

path_tracing::dx::wrapper::texture2d::texture2d(
	const std::shared_ptr<device>& device,
	const D3D12_RESOURCE_STATES& state,
	const D3D12_RESOURCE_FLAGS& flags,
	const D3D12_HEAP_TYPE& type,
	const DXGI_FORMAT& format, size_t width, size_t height) :
	mWidth(width), mHeight(height), mFormat(format)
{
	D3D12_RESOURCE_DESC desc;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = static_cast<UINT64>(mWidth);
	desc.Height = static_cast<UINT>(mHeight);
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = mFormat;
	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = flags;

	D3D12_HEAP_PROPERTIES properties = {
		type,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0,
	};

	(*device)->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE,
		&desc, state, nullptr, IID_PPV_ARGS(mResource.GetAddressOf()));
}

path_tracing::dx::wrapper::texture2d::texture2d(
	const ComPtr<ID3D12Resource>& resource, 
	const DXGI_FORMAT& format, size_t width, size_t height) :
	mWidth(width), mHeight(height), mFormat(format)
{
	mResource = resource;
}

DXGI_FORMAT path_tracing::dx::wrapper::texture2d::format() const noexcept
{
	return mFormat;
}

size_t path_tracing::dx::wrapper::texture2d::width() const noexcept
{
	return mWidth;
}

size_t path_tracing::dx::wrapper::texture2d::height() const noexcept
{
	return mHeight;
}

size_t path_tracing::dx::wrapper::texture2d::alignment() const noexcept
{
	return align_to(mWidth * size_of(mFormat), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
}

void path_tracing::dx::wrapper::texture2d::upload(
	const std::shared_ptr<graphics_command_list>& command_list, const std::shared_ptr<buffer>& buffer) const
{
	D3D12_TEXTURE_COPY_LOCATION dest;
	D3D12_TEXTURE_COPY_LOCATION src;

	dest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dest.SubresourceIndex = 0;
	dest.pResource = mResource.Get();

	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Format = mFormat;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(mWidth);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(mHeight);
	src.PlacedFootprint.Footprint.Depth = 1;
	src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(alignment());
	src.pResource = buffer->get();

	D3D12_BOX region = {
		0, 0, 0,
		static_cast<UINT>(mWidth),
		static_cast<UINT>(mHeight),
		1
	};

	(*command_list)->CopyTextureRegion(&dest, 0, 0, 0, &src, &region);
}