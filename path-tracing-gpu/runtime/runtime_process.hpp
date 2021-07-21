#pragma once

#include "runtime_service.hpp"

namespace path_tracing::runtime {

	class runtime_process final : public noncopyable {
	public:
		runtime_process();

		~runtime_process() = default;

		void run_loop();
		
		runtime_service service() const noexcept;
		
		friend struct runtime_service;
	private:
		runtime_service mRuntimeService;

		resources::resource_system mResourceSystem;
		resources::meshes_system mMeshesSystem;
		resources::images_system mImagesSystem;

		output::output_system mOutputSystem;
		inputs::inputs_system mInputsSystem;
		
		render::render_device mRenderDevice;
		render::render_system mRenderSystem;

		windows::window_system mWindowSystem;
		
		scenes::scene mScene;
	};
	
}
