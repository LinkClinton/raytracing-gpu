#include "render_device.hpp"

path_tracing::cores::render_device::render_device(const D3D_FEATURE_LEVEL& level)
{
	mDevice = wrapper::directx12::device::create(level);
	mFence = wrapper::directx12::fence::create(mDevice, 0);

	mMainQueue = wrapper::directx12::command_queue::create(mDevice);
}

void path_tracing::cores::render_device::wait() const
{
	mMainQueue.wait(mFence);
}

wrapper::directx12::command_queue path_tracing::cores::render_device::queue() const noexcept
{
	return mMainQueue;
}

wrapper::directx12::device path_tracing::cores::render_device::device() const noexcept
{
	return mDevice;
}

wrapper::directx12::fence path_tracing::cores::render_device::fence() const noexcept
{
	return mFence;
}
