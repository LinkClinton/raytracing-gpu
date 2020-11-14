#include "render_system.hpp"

#include "../../renderers/depth_renderer.hpp"

#include "../runtime_service.hpp"

void path_tracing::runtime::render::render_system::resolve(const runtime_service& service)
{
	mRenderer = std::make_shared<renderers::depth_renderer>(service);

	mRenderTarget = wrapper::directx12::texture2d::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(),
		DXGI_FORMAT_R32G32B32_FLOAT,
		static_cast<size_t>(service.scene.camera.resolution.x),
		static_cast<size_t>(service.scene.camera.resolution.y)
	);
}

void path_tracing::runtime::render::render_system::update(const runtime_service& service, real delta)
{
	if (mRenderer == nullptr) return;

	mRenderer->update(service, delta);
}

void path_tracing::runtime::render::render_system::render(const runtime_service& service, real delta)
{
	if (mRenderer == nullptr) return;
	
	mRenderer->render(service, delta);
}