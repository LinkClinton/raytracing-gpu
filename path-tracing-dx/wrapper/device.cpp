#include "device.hpp"

ID3D12Device5* const* path_tracing::dx::wrapper::device::get_address_of() const
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

path_tracing::dx::wrapper::device path_tracing::dx::wrapper::device::create()
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
