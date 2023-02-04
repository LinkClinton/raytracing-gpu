#pragma once

#include "resources/resource_system.hpp"
#include "output/output_system.hpp"

#include "render/render_device.hpp"
#include "render/render_system.hpp"

#include "windows/window_system.hpp"

#include "../scenes/scene.hpp"

namespace raytracing::runtime {

	class runtime_process;
	
	struct runtime_service final {
		resources::resource_system& resource_system;

		output::output_system& output_system;
		
		render::render_device& render_device;
		render::render_system& render_system;

		windows::window_system& window_system;

		scenes::scene& scene;

		runtime_service(runtime_process& process);
	};
	
}
