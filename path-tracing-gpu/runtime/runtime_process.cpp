#include "runtime_process.hpp"

path_tracing::runtime::runtime_process::runtime_process() :
	mRuntimeService(*this), mRenderDevice(D3D_FEATURE_LEVEL_12_0)
{
}

path_tracing::runtime::runtime_service path_tracing::runtime::runtime_process::service() const noexcept
{
	return mRuntimeService;
}
