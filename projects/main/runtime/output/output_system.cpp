#include "output_system.hpp"

#include "../../extensions/spdlog/spdlog.hpp"

#include "../runtime_service.hpp"
#include "../runtime_frame.hpp"

void raytracing::runtime::output::output_system::resolve(const runtime_service& service)
{
	extensions::spdlog::info("start rendering scene.");
}

void raytracing::runtime::output::output_system::release(const runtime_service& service)
{
	extensions::spdlog::info("finished rendering scene.");
	extensions::spdlog::info("time cost : {0}s.", mTotalTime);

	if (!service.window_system.active()) system("pause");
}

void raytracing::runtime::output::output_system::update(const runtime_service& service, const runtime_frame& frame)
{
	if (service.window_system.active()) 
	{
		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::Begin("Basic Info");
		ImGui::Text("Sample Index  : %lu", service.render_system.sample_index() - 1);
		ImGui::Text("Frame  Index  : %llu", frame.frame_index);
		ImGui::Text("Delta  Time   : %f", frame.delta_time);
		ImGui::Text("Total  Time   : %f", frame.total_time);
		ImGui::Text("FPS	      : %f", ImGui::GetIO().Framerate);
		ImGui::Separator();
		ImGui::End();
	}
	else extensions::spdlog::info("finished sample index {0} cost {1}s.", frame.frame_index, frame.delta_time);

	// record the total time of rendering 
	mTotalTime = frame.total_time;
}
