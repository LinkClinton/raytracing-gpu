#include "wrapper_dx.hpp"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

uint64_t align_to(uint64_t value, uint64_t alignment)
{
	return ((value + alignment - 1) / alignment) * alignment;
}

path_tracing::dx::device path_tracing::dx::device::create()
{
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debug;

	D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

	debug->EnableDebugLayer();
#endif

	device device;
	
	D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(device.mDevice.GetAddressOf()));

	return device;
}

path_tracing::dx::command_allocator path_tracing::dx::command_allocator::create(const device& device)
{
	command_allocator allocator;

	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(allocator.mAllocator.GetAddressOf()));

	return allocator;
}

path_tracing::dx::graphics_command_list path_tracing::dx::graphics_command_list::create(
	const device& device, const command_allocator& allocator)
{
	graphics_command_list command_list;

	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.get(), nullptr,
		IID_PPV_ARGS(command_list.mCommandList.GetAddressOf()));

	return command_list;
}

path_tracing::dx::command_queue path_tracing::dx::command_queue::create(const device& device)
{
	command_queue queue;

	D3D12_COMMAND_QUEUE_DESC desc;

	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = 0;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	
	device->CreateCommandQueue(&desc, IID_PPV_ARGS(queue.mCommandQueue.GetAddressOf()));

	return queue;
}

path_tracing::dx::swap_chain path_tracing::dx::swap_chain::create(
	const command_queue& queue, int width, int height, void* handle)
{
	swap_chain swap_chain;

	DXGI_SWAP_CHAIN_DESC desc;

	desc.BufferCount = 2;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.Width = static_cast<UINT>(width);
	desc.BufferDesc.Height = static_cast<UINT>(height);
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.OutputWindow = static_cast<HWND>(handle);
	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;
	desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Windowed = true;

	ComPtr<IDXGIFactory4> factory;
	ComPtr<IDXGISwapChain> temp_swap_chain;

	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));

	factory->CreateSwapChain(queue.get(), &desc, temp_swap_chain.GetAddressOf());

	temp_swap_chain->QueryInterface(IID_PPV_ARGS(swap_chain.mSwapChain.GetAddressOf()));

	return swap_chain;
}

D3D12_RESOURCE_BARRIER path_tracing::dx::resource::barrier_transition(
	const D3D12_RESOURCE_STATES& before, 
	const D3D12_RESOURCE_STATES& after,
	const resource& resource)
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

size_t path_tracing::dx::buffer::size() const noexcept
{
	return mSize;
}

void* path_tracing::dx::buffer::map() const
{
	void* data = nullptr;

	mResource->Map(0, nullptr, &data);

	return data;
}

void path_tracing::dx::buffer::unmap() const
{
	mResource->Unmap(0, nullptr);
}

path_tracing::dx::buffer path_tracing::dx::buffer::create(
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

D3D12_RESOURCE_DESC path_tracing::dx::buffer::resource_desc(size_t size, const D3D12_RESOURCE_FLAGS& flags)
{
	D3D12_RESOURCE_DESC desc;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = static_cast<UINT64>(size);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = flags;
	desc.Alignment = 0;
	
	return desc;
}

path_tracing::dx::buffer path_tracing::dx::buffer::upload_type(
	const D3D12_RESOURCE_STATES& state, 
	const D3D12_RESOURCE_FLAGS& flags,
	const device& device, size_t size)
{
	const D3D12_HEAP_PROPERTIES properties = {
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0,
	};
	
	return create(state, properties, resource_desc(size, flags), device);
}

path_tracing::dx::buffer path_tracing::dx::buffer::default_type(
	const D3D12_RESOURCE_STATES& state, 
	const D3D12_RESOURCE_FLAGS& flags,
	const device& device, size_t size)
{
	const D3D12_HEAP_PROPERTIES properties = {
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0,
	};

	return create(state, properties, resource_desc(size, flags), device);
}

size_t path_tracing::dx::format_size(const DXGI_FORMAT& format)
{
	switch (format) {
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return 4;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return 16;
	default:
		return 0;
	}
}

DXGI_FORMAT path_tracing::dx::texture2d::format() const noexcept
{
	return mFormat;
}

size_t path_tracing::dx::texture2d::width() const noexcept
{
	return mWidth;
}

size_t path_tracing::dx::texture2d::height() const noexcept
{
	return mHeight;
}

size_t path_tracing::dx::texture2d::alignment() const noexcept
{
	return align_to(mWidth * format_size(mFormat), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);
}

void path_tracing::dx::texture2d::upload(const graphics_command_list& command_list, const buffer& buffer) const
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

path_tracing::dx::texture2d path_tracing::dx::texture2d::create(
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

path_tracing::dx::texture2d path_tracing::dx::texture2d::default_type(
	const D3D12_RESOURCE_STATES& state, 
	const D3D12_RESOURCE_FLAGS& flags, 
	const DXGI_FORMAT& format,
	const device& device, 
	size_t width, size_t height)
{
	const D3D12_HEAP_PROPERTIES properties = {
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0,
	};

	D3D12_RESOURCE_DESC desc;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = static_cast<UINT64>(width);
	desc.Height = static_cast<UINT>(height);
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = format;
	desc.SampleDesc.Quality = 0;
	desc.SampleDesc.Count = 1;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = flags;
	desc.Alignment = 0;

	return create(state, properties, desc, device);
}
