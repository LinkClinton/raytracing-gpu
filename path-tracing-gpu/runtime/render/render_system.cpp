#include "render_system.hpp"

#include "../../extensions/images/image_stb.hpp"
#include "../../renderers/module_renderer.hpp"
#include "../../renderers/depth_renderer.hpp"

#include "../runtime_service.hpp"
#include "../runtime_frame.hpp"

void path_tracing::runtime::render::render_system::resolve(const runtime_service& service)
{
	mRenderTargetHDR = wrapper::directx12::texture2d::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		service.scene.film.size_x,
		service.scene.film.size_y
	);

	mRenderTargetSDR = wrapper::directx12::texture2d::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		DXGI_FORMAT_R8G8B8A8_UNORM,
		service.scene.film.size_x,
		service.scene.film.size_y
	);
	
	service.resource_system.add<wrapper::directx12::texture2d>("RenderSystem.RenderTarget.HDR", mRenderTargetHDR);
	service.resource_system.add<wrapper::directx12::texture2d>("RenderSystem.RenderTarget.SDR", mRenderTargetSDR);

	mRenderer = std::make_shared<renderers::module_renderer>(
		std::vector<renderers::submodule>{
			renderers::submodule{ "./materials/material_substrate.hlsl", "substrate_material" },
			renderers::submodule{ "./materials/material_diffuse.hlsl", "diffuse_material" },
			renderers::submodule{ "./materials/material_mirror.hlsl", "mirror_material" },
			renderers::submodule{ "./materials/material_glass.hlsl", "glass_material" },
			renderers::submodule{ "./materials/material_metal.hlsl", "metal_material" }
		},
		std::vector<renderers::submodule>{
			renderers::submodule{ "./lights/light_environment.hlsl", "environment_light" },
			renderers::submodule{ "./lights/light_directional.hlsl", "directional_light" },
			renderers::submodule{ "./lights/light_surface.hlsl", "surface_light" }
		}, service);
}

void path_tracing::runtime::render::render_system::release(const runtime_service& service)
{
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

path_tracing::uint32 path_tracing::runtime::render::render_system::sample_index() const noexcept
{
	return mRenderer->sample_index();
}
