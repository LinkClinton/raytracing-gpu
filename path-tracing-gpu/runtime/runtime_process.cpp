#include "runtime_process.hpp"
#include "runtime_frame.hpp"

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

	runtime_frame frame;
	
	while (mWindowSystem.living() || frame.frame_index < mRuntimeService.scene.sample_count) {
		auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(time_point::now() - current);

		current = time_point::now();

		frame.delta_time = duration.count();
		frame.total_time = frame.total_time + duration.count();
		
		mRenderSystem.update(mRuntimeService, frame);
		mWindowSystem.update(mRuntimeService, frame);

		mRenderDevice.wait();
		
		mRenderSystem.render(mRuntimeService, frame);
		mWindowSystem.render(mRuntimeService, frame);

		frame.frame_index++;
	}

	mRenderDevice.wait();
}

path_tracing::runtime::runtime_service path_tracing::runtime::runtime_process::service() const noexcept
{
	return mRuntimeService;
}
