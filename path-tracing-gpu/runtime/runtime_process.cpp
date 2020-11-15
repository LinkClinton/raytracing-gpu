#include "runtime_process.hpp"

path_tracing::runtime::runtime_process::runtime_process() :
	mRuntimeService(*this), mRenderDevice(D3D_FEATURE_LEVEL_12_0)
{
}

void path_tracing::runtime::runtime_process::run_loop()
{
	using time_point = std::chrono::high_resolution_clock;

	mRenderSystem.resolve(mRuntimeService);
	mWindowSystem.resolve(mRuntimeService);
	
	auto current = time_point::now();
	
	while (mWindowSystem.living()) {
		auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(time_point::now() - current);

		current = time_point::now();

		mRenderSystem.update(mRuntimeService, duration.count());
		mRenderSystem.render(mRuntimeService, duration.count());
		
		mWindowSystem.update(mRuntimeService, duration.count());
		mWindowSystem.render(mRuntimeService, duration.count());
		
		mRenderDevice.wait();
	}

	mRenderDevice.wait();
}

path_tracing::runtime::runtime_service path_tracing::runtime::runtime_process::service() const noexcept
{
	return mRuntimeService;
}
