#include "render_system.hpp"

#include "../runtime_service.hpp"
#include "../runtime_frame.hpp"

#include "../../renderers/renderer.hpp"

void raytracing::runtime::render::render_system::resolve(const runtime_service& service)
{
	mRenderTargetHDR.info = resources::texture_info
	{
		service.scene.film.size_x,
		service.scene.film.size_y,
		4,
		resources::texture_format::fp32
	};

	mRenderTargetHDR.data = wrapper::directx12::texture2d::create
	(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		service.scene.film.size_x,
		service.scene.film.size_y
	);

	mRenderTargetSDR.info = resources::texture_info
	{
		service.scene.film.size_x,
		service.scene.film.size_y,
		4,
		resources::texture_format::uint8
	};

	mRenderTargetSDR.data = wrapper::directx12::texture2d::create
	(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		DXGI_FORMAT_R8G8B8A8_UNORM,
		service.scene.film.size_x,
		service.scene.film.size_y
	);
	
	service.resource_system.add<resources::gpu_texture>("RenderSystem.RenderTarget.HDR", mRenderTargetHDR);
	service.resource_system.add<resources::gpu_texture>("RenderSystem.RenderTarget.SDR", mRenderTargetSDR);
}

void raytracing::runtime::render::render_system::release(const runtime_service& service)
{
}

void raytracing::runtime::render::render_system::update(const runtime_service& service, const runtime_frame& frame)
{
	if (mRenderer == nullptr) return;

	mRenderer->update(service, frame);
}

void raytracing::runtime::render::render_system::render(const runtime_service& service, const runtime_frame& frame)
{
	if (mRenderer == nullptr) return;

	mRenderer->render(service, frame);
}

raytracing::uint32 raytracing::runtime::render::render_system::sample_index() const noexcept
{
	return mRenderer ? mRenderer->sample_index() : 1;
}
