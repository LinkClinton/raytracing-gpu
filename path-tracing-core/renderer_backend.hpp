#pragma once

#include "cameras/camera.hpp"
#include "scenes/scene.hpp"

namespace path_tracing::core {

	using namespace cameras;
	using namespace scenes;

	struct render_config {
		size_t width = 0;
		size_t height = 0;
		real scale = 1;

		render_config() = default;

		render_config(size_t width, size_t height, real scale);
	};
	
	class renderer_backend : interfaces::noncopyable {
	public:
		explicit renderer_backend(void* handle, int width, int height);

		~renderer_backend() = default;

		virtual void render(const std::shared_ptr<camera>& camera) = 0;

		virtual void build(const std::shared_ptr<scene>& scene, const render_config& config) = 0;

		virtual void resize(int new_width, int new_height) = 0;

		virtual void release() = 0;
	protected:
		void* mHandle;

		int mWidth, mHeight;
	};
	
}
