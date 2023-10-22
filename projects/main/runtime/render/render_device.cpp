#include "render_device.hpp"

raytracing::runtime::render::render_device::render_device(const D3D_FEATURE_LEVEL& level)
{
	wrapper::directx12::device::enable_debug_layer();

	mDevice = wrapper::directx12::device::create(level);
	mFence = wrapper::directx12::fence::create(mDevice, 0);

	mMainQueue = wrapper::directx12::command_queue::create(mDevice);
}

void raytracing::runtime::render::render_device::wait() const
{
	mMainQueue.wait(mFence);
}

wrapper::directx12::command_queue raytracing::runtime::render::render_device::queue() const noexcept
{
	return mMainQueue;
}

wrapper::directx12::device raytracing::runtime::render::render_device::device() const noexcept
{
	return mDevice;
}

wrapper::directx12::fence raytracing::runtime::render::render_device::fence() const noexcept
{
	return mFence;
}
