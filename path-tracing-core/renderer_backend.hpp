#pragma once

#include "cameras/camera.hpp"
#include "scenes/scene.hpp"

namespace path_tracing::core {

	using namespace cameras;
	using namespace scenes;
	
	class renderer_backend : interfaces::noncopyable {
	public:
		explicit renderer_backend(void* handle, int width, int height);

		~renderer_backend() = default;

		virtual void render(const std::shared_ptr<camera>& camera) = 0;

		virtual void build(const std::shared_ptr<scene>& scene) = 0;

		virtual void resize(int new_width, int new_height) = 0;
	protected:
		void* mHandle;

		int mWidth, mHeight;
	};
	
}
