#include "device.hpp"

path_tracing::dx::wrapper::device::device(const D3D_FEATURE_LEVEL& level)
{
#ifdef _DEBUG
	ComPtr<ID3D12Debug> debug;

	D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf()));

	debug->EnableDebugLayer();
#endif

	D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(mDevice.GetAddressOf()));
}

ID3D12Device5* const* path_tracing::dx::wrapper::device::get_address_off() const
{
	return mDevice.GetAddressOf();
}

ID3D12Device5* path_tracing::dx::wrapper::device::operator->() const
{
	return mDevice.Get();
}

ID3D12Device5* path_tracing::dx::wrapper::device::get() const
{
	return mDevice.Get();
}
