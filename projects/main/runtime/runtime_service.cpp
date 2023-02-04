#include "runtime_service.hpp"

#include "runtime_process.hpp"

raytracing::runtime::runtime_service::runtime_service(runtime_process& process) :
	resource_system(process.mResourceSystem),
	output_system(process.mOutputSystem),
	render_device(process.mRenderDevice),
	render_system(process.mRenderSystem),
	window_system(process.mWindowSystem),
	scene(process.mScene)
{
}
