#include "render_system.hpp"

#include "../../extensions/images/image_stb.hpp"
#include "../../renderers/depth_renderer.hpp"

#include "../runtime_service.hpp"
#include "../runtime_frame.hpp"

void path_tracing::runtime::render::render_system::resolve(const runtime_service& service)
{
	mRenderTargetHDR = wrapper::directx12::texture2d::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		static_cast<size_t>(service.scene.camera.resolution.x),
		static_cast<size_t>(service.scene.camera.resolution.y)
	);

	mRenderTargetSDR = wrapper::directx12::texture2d::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		DXGI_FORMAT_R8G8B8A8_UNORM,
		static_cast<size_t>(service.scene.camera.resolution.x),
		static_cast<size_t>(service.scene.camera.resolution.y)
	);
	
	service.resource_system.add<wrapper::directx12::texture2d>("RenderSystem.RenderTarget.HDR", mRenderTargetHDR);
	service.resource_system.add<wrapper::directx12::texture2d>("RenderSystem.RenderTarget.SDR", mRenderTargetSDR);

	mRenderer = std::make_shared<renderers::depth_renderer>(service);
}

void path_tracing::runtime::render::render_system::update(const runtime_service& service, const runtime_frame& frame)
{
	if (mRenderer == nullptr) return;

	mRenderer->update(service, frame);
}

void path_tracing::runtime::render::render_system::render(const runtime_service& service, const runtime_frame& frame)
{
	if (mRenderer == nullptr) return;
	
	mRenderer->render(service, frame);
}

void path_tracing::runtime::render::render_system::save_render_target_sdr(const render_device& device, const std::string& filename) const
{
	const auto command_allocator = wrapper::directx12::command_allocator::create(device.device());
	const auto command_list = wrapper::directx12::graphics_command_list::create(device.device(), command_allocator);

	command_allocator->Reset();
	command_list->Reset(command_allocator.get(), nullptr);

	const auto render_target_read_buffer = wrapper::directx12::buffer::create(
		device.device(),
		wrapper::directx12::resource_info::readback(),
		mRenderTargetSDR.alignment() * mRenderTargetSDR.size_y()
	);
	
	render_target_read_buffer.copy_from(command_list, mRenderTargetSDR);
	
	command_list.close();
	
	device.queue().execute({ command_list });
	device.wait();

	// see https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
	// DXGI_FORMAT_XXX_UNORM: Unsigned normalized integer; which is interpreted in a resource as an unsigned integer
	// and is interpreted in a shader as an unsigned normalized floating-point value in the range [0, 1].
	auto cpu_image_data = std::vector<byte>(mRenderTargetSDR.size_x() * mRenderTargetSDR.size_y() * 4);
	auto gpu_image_data = static_cast<byte*>(render_target_read_buffer.begin_mapping());

	for (size_t y = 0; y < mRenderTargetSDR.size_y(); y++) {
		std::memcpy(cpu_image_data.data() + y * mRenderTargetSDR.size_x() * 4,
			gpu_image_data + y * mRenderTargetSDR.alignment(),
			mRenderTargetSDR.size_x() * 4);
	}
	
	render_target_read_buffer.end_mapping();

	extensions::images::write_png_image_to_file(filename, {
		cpu_image_data,
		mRenderTargetSDR.size_x(),
		mRenderTargetSDR.size_y()
	});
}
