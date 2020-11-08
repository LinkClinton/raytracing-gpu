#pragma once

#include "runtime_service.hpp"

namespace path_tracing::runtime {

	class runtime_process final : public noncopyable {
	public:
		runtime_process();

		~runtime_process() = default;
	
		runtime_service service() const noexcept;
		
		friend struct runtime_service;
	private:
		runtime_service mRuntimeService;

		resources::meshes_system mMeshesSystem;
		
		render::render_device mRenderDevice;

		scenes::scene mScene;
	};
	
}
