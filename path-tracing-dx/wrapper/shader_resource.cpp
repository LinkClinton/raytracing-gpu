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

D3D12_RESOURCE_BARRIER path_tracing::dx::wrapper::shader_resource::barrier_transition(
	const D3D12_RESOURCE_STATES& before, 
	const D3D12_RESOURCE_STATES& after,
	const shader_resource& resource)
{
	D3D12_RESOURCE_BARRIER barrier;

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.pResource = resource.get();

	return barrier;
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

path_tracing::dx::wrapper::buffer path_tracing::dx::wrapper::buffer::create(
	const D3D12_RESOURCE_STATES& state,
	const D3D12_HEAP_PROPERTIES& heap,
	const D3D12_RESOURCE_DESC& desc,
	const device& device)
{
	buffer buffer;

	buffer.mSize = static_cast<size_t>(desc.Width);

	device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, state,
		nullptr, IID_PPV_ARGS(buffer.mResource.GetAddressOf()));

	return buffer;
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

void path_tracing::dx::wrapper::texture2d::upload(const graphics_command_list& command_list, const buffer& buffer) const
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
	src.pResource = buffer.get();

	D3D12_BOX region = {
		0, 0, 0,
		static_cast<UINT>(mWidth),
		static_cast<UINT>(mHeight),
		1
	};

	command_list->CopyTextureRegion(&dest, 0, 0, 0, &src, &region);
}

path_tracing::dx::wrapper::texture2d path_tracing::dx::wrapper::texture2d::create(
	const D3D12_RESOURCE_STATES& state,
	const D3D12_HEAP_PROPERTIES& heap,
	const D3D12_RESOURCE_DESC& desc,
	const device& device)
{
	texture2d texture;

	texture.mWidth = static_cast<size_t>(desc.Width);
	texture.mHeight = static_cast<size_t>(desc.Height);
	texture.mFormat = desc.Format;

	device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &desc, state,
		nullptr, IID_PPV_ARGS(texture.mResource.GetAddressOf()));

	return texture;
}

path_tracing::dx::wrapper::texture2d path_tracing::dx::wrapper::texture2d::create(
	ComPtr<ID3D12Resource>& resource,
	const DXGI_FORMAT& format, size_t width, size_t height)
{
	texture2d texture;

	texture.mWidth = width;
	texture.mHeight = height;
	texture.mFormat = format;
	texture.mResource = resource;

	return texture;
}