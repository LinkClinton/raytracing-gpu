#include "swap_chain.hpp"

path_tracing::dx::wrapper::swap_chain::swap_chain(
	const std::shared_ptr<command_queue>& queue,
	const std::shared_ptr<device>& device, 
	size_t width, size_t height, void* handle)
{
	mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	mBufferCount = 2;

	DXGI_SWAP_CHAIN_DESC desc;

	desc.BufferCount = static_cast<UINT>(mBufferCount);
	desc.BufferDesc.Format = mFormat;
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

	factory->CreateSwapChain(queue->get(), &desc, temp_swap_chain.GetAddressOf());

	temp_swap_chain->QueryInterface(IID_PPV_ARGS(mSwapChain.GetAddressOf()));

	resize(device, width, height);
}

IDXGISwapChain4* const* path_tracing::dx::wrapper::swap_chain::get_address_of() const
{
	return mSwapChain.GetAddressOf();
}

IDXGISwapChain4* path_tracing::dx::wrapper::swap_chain::operator->() const
{
	return mSwapChain.Get();
}

IDXGISwapChain4* path_tracing::dx::wrapper::swap_chain::get() const
{
	return mSwapChain.Get();
}

void path_tracing::dx::wrapper::swap_chain::resize(const std::shared_ptr<device>& device, size_t width, size_t height)
{
	mSwapChain->ResizeBuffers(
		static_cast<UINT>(mBufferCount), static_cast<UINT>(width), static_cast<UINT>(height),
		mFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	mBackBuffers.clear();

	for (size_t index = 0; index < mBufferCount; index++) {
		ComPtr<ID3D12Resource> backBuffer;

		mSwapChain->GetBuffer(static_cast<UINT>(index), IID_PPV_ARGS(backBuffer.GetAddressOf()));

		mBackBuffers.push_back(std::make_shared<texture2d>(backBuffer, mFormat, width, height));
	}
}

void path_tracing::dx::wrapper::swap_chain::present(bool sync) const
{
	mSwapChain->Present(sync ? 1 : 0, 0);
}

std::shared_ptr<path_tracing::dx::wrapper::texture2d> path_tracing::dx::wrapper::swap_chain::buffer(size_t index) const
{
	return mBackBuffers[index];
}

DXGI_FORMAT path_tracing::dx::wrapper::swap_chain::format() const noexcept
{
	return mFormat;
}

size_t path_tracing::dx::wrapper::swap_chain::current_frame_index() const
{
	return mSwapChain->GetCurrentBackBufferIndex();
}

size_t path_tracing::dx::wrapper::swap_chain::count() const noexcept
{
	return mBufferCount;
}
