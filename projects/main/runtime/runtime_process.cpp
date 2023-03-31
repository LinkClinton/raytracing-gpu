#include "runtime_process.hpp"
#include "runtime_frame.hpp"

#include <chrono>

raytracing::runtime::runtime_process::runtime_process() :
	mRuntimeService(*this), mRenderDevice(D3D_FEATURE_LEVEL_12_1)
{
}

void raytracing::runtime::runtime_process::run_loop()
{
	using time_point = std::chrono::high_resolution_clock;

	mRenderSystem.resolve(mRuntimeService);
	mWindowSystem.resolve(mRuntimeService);
	mOutputSystem.resolve(mRuntimeService);
	
	auto current = time_point::now();

	runtime_frame frame;

	// renderer will record sample_index by itself so sample_index is not equal frame_index
	// but in console rendering mode(without window system), frame_index is equal to sample_index
	// todo : using sample_index instead of frame_index to break loop
	while (mWindowSystem.living() || (!mWindowSystem.active() && frame.frame_index < mRuntimeService.scene.sample_count)) 
	{
		auto duration = std::chrono::duration_cast<std::chrono::duration<float>>(time_point::now() - current);

		current = time_point::now();

		frame.delta_time = duration.count();
		frame.total_time = frame.total_time + duration.count();

		mRenderSystem.update(mRuntimeService, frame);
		mWindowSystem.update(mRuntimeService, frame);
		mOutputSystem.update(mRuntimeService, frame);

		mRenderDevice.wait();
		
		mRenderSystem.render(mRuntimeService, frame);
		mWindowSystem.render(mRuntimeService, frame);

		frame.frame_index++;
	}
	
	mRenderSystem.release(mRuntimeService);
	mWindowSystem.release(mRuntimeService);
	mOutputSystem.release(mRuntimeService);

	mRenderDevice.wait();
}

raytracing::runtime::runtime_service raytracing::runtime::runtime_process::service() const noexcept
{
	return mRuntimeService;
}
