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

		resources::meshes_system mMeshesSystem;

		render::render_device mRenderDevice;
		render::render_system mRenderSystem;
		
		windows::view_window mViewWindow;
		
		scenes::scene mScene;
	};
	
}
