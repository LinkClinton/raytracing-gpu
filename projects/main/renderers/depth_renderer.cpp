#include "depth_renderer.hpp"

raytracing::renderers::depth_renderer::depth_renderer(const runtime_service& service) : renderer(service)
{
	mCommandAllocator = wrapper::directx12::command_allocator::create(service.render_device.device());
	mCommandList = wrapper::directx12::graphics_command_list::create(service.render_device.device(), mCommandAllocator);

	mCommandAllocator.reset();
	mCommandList.reset(mCommandAllocator);

}

void raytracing::renderers::depth_renderer::update(const runtime_service& service, const runtime_frame& frame)
{
}

void raytracing::renderers::depth_renderer::render(const runtime_service& service, const runtime_frame& frame)
{
}
