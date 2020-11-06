#include "runtime_service.hpp"

#include "runtime_process.hpp"

path_tracing::runtime::runtime_service::runtime_service(runtime_process& process) :
	meshes_system(process.mMeshesSystem), render_device(process.mRenderDevice)
{
}
