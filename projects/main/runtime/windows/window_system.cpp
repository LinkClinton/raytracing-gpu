#include "window_system.hpp"

#include "../runtime_service.hpp"
#include "../runtime_frame.hpp"

void raytracing::runtime::windows::window_system::resolve(const runtime_service& service)
{
	if (service.scene.output_window.has_value() && service.scene.output_window->enable) 
	{
		mViewWindow = view_window(
			service.render_device.queue(), service.render_device.device(),
			service.scene.output_window->name,
			static_cast<uint32>(service.scene.output_window->size_x),
			static_cast<uint32>(service.scene.output_window->size_y));

		mImGuiRenderer.resolve(mViewWindow.mSwapChain, service.render_device, service.scene.output_window->font_size);

		mRenderTargetID = mImGuiRenderer.allocate(
			service.resource_system.resource<resources::gpu_texture>("RenderSystem.RenderTarget.SDR").data,
			1);
	}
}

void raytracing::runtime::windows::window_system::release(const runtime_service& service)
{
}

void raytracing::runtime::windows::window_system::update(const runtime_service& service, const runtime_frame& frame)
{
	if (mViewWindow.living() == false) return;
	
	mViewWindow.update(service, frame);
	
	mImGuiRenderer.update(service, frame);

	ImGui::GetBackgroundDrawList()->AddImage(mRenderTargetID, ImVec2(0, 0),
		ImVec2(
			static_cast<real>(mViewWindow.size_x()),
			static_cast<real>(mViewWindow.size_y())));
}

void raytracing::runtime::windows::window_system::render(const runtime_service& service, const runtime_frame& frame)
{
	if (mViewWindow.living() == false) return;

	mImGuiRenderer.render(service, frame);

	mViewWindow.present(false);
}

bool raytracing::runtime::windows::window_system::living() const noexcept
{
	return mViewWindow.living();
}

bool raytracing::runtime::windows::window_system::active() const noexcept
{
	return mViewWindow.mHandle != nullptr;
}
