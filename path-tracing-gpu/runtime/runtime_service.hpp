#pragma once

#include "resources/meshes_system.hpp"

#include "render/render_device.hpp"

namespace path_tracing::runtime {

	class runtime_process;
	
	struct runtime_service final {
		resources::meshes_system& meshes_system;
		
		render::render_device& render_device;
		
		runtime_service(runtime_process& process);
	};
	
}
