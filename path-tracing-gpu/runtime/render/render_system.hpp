#pragma once

#include "render_device.hpp"

#include <memory>

namespace path_tracing::renderers {

	class renderer;
	
}

namespace path_tracing::runtime {

	struct runtime_service;
	
}

namespace path_tracing::runtime::render {

	using runtime::runtime_service;

	class render_system final : public noncopyable {
	public:
		render_system() = default;

		~render_system() = default;

		void resolve(const runtime_service& service);
		
		void update(const runtime_service& service, real delta);

		void render(const runtime_service& service, real delta);
	private:
		std::shared_ptr<renderers::renderer> mRenderer;

		wrapper::directx12::texture2d mRenderTarget;
	};
	
}
