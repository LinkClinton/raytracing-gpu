#include "renderer_directx.hpp"

path_tracing::dx::renderer_directx::renderer_directx(void* handle, int width, int height) :
	renderer_backend(handle, width, height)
{
	mDevice = device::create();

	mCommandAllocator = command_allocator::create(mDevice);
	mCommandQueue = command_queue::create(mDevice);

	mCommandList = graphics_command_list::create(mDevice, mCommandAllocator);

	mSwapChain = swap_chain::create(mDevice, mCommandQueue, mWidth, mHeight, mHandle);
}

void path_tracing::dx::renderer_directx::render(const std::shared_ptr<core::camera>& camera)
{
	// todo:
	
	mSwapChain.present();

	mCommandQueue.wait();
}

void path_tracing::dx::renderer_directx::build(const std::shared_ptr<core::scene>& scene)
{
	// todo:
}

void path_tracing::dx::renderer_directx::resize(int new_width, int new_height)
{
	mSwapChain.resize(mDevice, new_width, new_height);
}
