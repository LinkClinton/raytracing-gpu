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
	extensions::spdlog::info("time cost : {0}s.", mTotalTime);
	
	// when scene.output_images is not null, we will output the images to files.
	if (service.scene.output_images.has_value() && !service.scene.output_images->sdr_image.empty()) {
		const auto render_target_sdr = service.resource_system.resource<
			wrapper::directx12::texture2d>("RenderSystem.RenderTarget.SDR");
		
		extensions::images::write_image_to_png(service.scene.output_images->sdr_image, render_target_sdr, service.render_device);
		
		extensions::spdlog::info("saved sdr image to file : {0}.", service.scene.output_images->sdr_image);
	}

	if (!service.window_system.active()) system("pause");
}

void path_tracing::runtime::output::output_system::update(const runtime_service& service, const runtime_frame& frame)
{
	if (service.window_system.active()) {
		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::Begin("Basic Info");
		ImGui::Text("Sample Index  : %lu", service.render_system.sample_index() - 1);
		ImGui::Text("Frame  Index  : %llu", frame.frame_index);
		ImGui::Text("Delta  Time   : %f", frame.delta_time);
		ImGui::Text("Total  Time   : %f", frame.total_time);
		ImGui::Text("FPS	      : %f", ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::Text("Max Depth    : %lu", service.scene.max_depth);
		ImGui::Text("SizeX        : %lu", static_cast<uint32>(service.scene.film.size_x));
		ImGui::Text("SizeY        : %lu", static_cast<uint32>(service.scene.film.size_y));
		ImGui::End();
	}
	else extensions::spdlog::info("finished sample index {0} cost {1}s.", frame.frame_index, frame.delta_time);

	// record the total time of rendering 
	mTotalTime = frame.total_time;
}
