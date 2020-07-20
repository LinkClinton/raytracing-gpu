#include "utility.hpp"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

size_t path_tracing::dx::wrapper::size_of(const DXGI_FORMAT& format)
{
	switch (format) {
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return 4;
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return 16;
	case DXGI_FORMAT_R32G32B32_FLOAT:
		return 12;
	case DXGI_FORMAT_R32G32_FLOAT:
		return 8;
	case DXGI_FORMAT_R32_FLOAT:
		return 4;
	default:
		return 0;
	}
}

size_t path_tracing::dx::wrapper::align_to(size_t value, size_t alignment)
{
	return ((value + alignment - 1) / alignment) * alignment;
}

D3D12_CPU_DESCRIPTOR_HANDLE path_tracing::dx::wrapper::offset_handle(const D3D12_CPU_DESCRIPTOR_HANDLE& handle, size_t value)
{
	return { handle.ptr + value };
}

D3D12_GPU_DESCRIPTOR_HANDLE path_tracing::dx::wrapper::offset_handle(const D3D12_GPU_DESCRIPTOR_HANDLE& handle, size_t value)
{
	return { handle.ptr + value };
}
