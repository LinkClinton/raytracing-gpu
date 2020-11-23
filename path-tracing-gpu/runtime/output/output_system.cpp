#include "output_system.hpp"

#include "../../extensions/images/image_stb.hpp"
#include "../../extensions/spdlog/spdlog.hpp"

#include "../runtime_service.hpp"
#include "../runtime_frame.hpp"

void path_tracing::runtime::output::output_system::resolve(const runtime_service& service)
{
	extensions::spdlog::info("start rendering scene.");
}

void path_tracing::runtime::output::output_system::release(const runtime_service& service)
{
	extensions::spdlog::info("finished rendering scene.");

	// when scene.output_images is not null, we will output the images to files.
	if (service.scene.output_images.has_value() && !service.scene.output_images->sdr_image.empty()) {
		const auto render_target_sdr = service.resource_system.resource<
			wrapper::directx12::texture2d>("RenderSystem.RenderTarget.SDR");
		
		extensions::images::write_image_to_png(service.scene.output_images->sdr_image, render_target_sdr, service.render_device);
		
		extensions::spdlog::info("saved sdr image to file : {0}.", service.scene.output_images->sdr_image);
	}
}

void path_tracing::runtime::output::output_system::update(const runtime_service& service, const runtime_frame& frame)
{
}
