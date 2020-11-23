#pragma once

#include "render_device.hpp"

#include <memory>

namespace path_tracing::renderers {

	class renderer;
	
}

namespace path_tracing::runtime {

	struct runtime_service;
	struct runtime_frame;
	
}

namespace path_tracing::runtime::render {

	using runtime::runtime_service;

	class render_system final : public noncopyable {
	public:
		render_system() = default;

		~render_system() = default;

		void resolve(const runtime_service& service);
		
		void update(const runtime_service& service, const runtime_frame& frame);

		void render(const runtime_service& service, const runtime_frame& frame);

		void save_render_target_sdr(const render_device& device, const std::string& filename) const;
	private:
		std::shared_ptr<renderers::renderer> mRenderer;

		wrapper::directx12::texture2d mRenderTargetHDR;
		wrapper::directx12::texture2d mRenderTargetSDR;
	};
	
}
