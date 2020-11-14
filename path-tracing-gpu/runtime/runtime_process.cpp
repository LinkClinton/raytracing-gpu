#include "runtime_process.hpp"

path_tracing::runtime::runtime_process::runtime_process() :
	mRuntimeService(*this), mRenderDevice(D3D_FEATURE_LEVEL_12_0)
{
}

void path_tracing::runtime::runtime_process::run_loop()
{
	using time_point = std::chrono::high_resolution_clock;
	
	if (!mScene.output_window.empty()) {
		mViewWindow = windows::view_window(mRenderDevice.queue(), mRenderDevice.device(), mScene.output_window,
			static_cast<uint32>(mScene.camera.resolution.x),
			static_cast<uint32>(mScene.camera.resolution.y));
	}

	mRenderSystem.resolve(mRuntimeService);
	
	auto current = time_point::now();
	
	while (mViewWindow.living()) {
		auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(time_point::now() - current);

		current = time_point::now();

		mRenderSystem.update(mRuntimeService, duration.count());
		mRenderSystem.render(mRuntimeService, duration.count());
		
		mViewWindow.update(duration.count());
		mViewWindow.present(false);

		mRenderDevice.wait();
	}

	mRenderDevice.wait();
	
	mViewWindow = windows::view_window();
}

path_tracing::runtime::runtime_service path_tracing::runtime::runtime_process::service() const noexcept
{
	return mRuntimeService;
}
