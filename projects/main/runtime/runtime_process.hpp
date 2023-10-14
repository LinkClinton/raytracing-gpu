#pragma once

#include "runtime_service.hpp"

namespace raytracing::runtime
{
	class runtime_process final : public noncopyable
	{
	public:
		runtime_process();

		void run_loop();
		
		runtime_service service() const noexcept;
		
		friend struct runtime_service;
	private:
		runtime_service mRuntimeService;

		resources::resource_system mResourceSystem;
		output::output_system mOutputSystem;
		
		render::render_device mRenderDevice;
		render::render_system mRenderSystem;

		windows::window_system mWindowSystem;

		scenes::scene mScene;
	};
	
}
