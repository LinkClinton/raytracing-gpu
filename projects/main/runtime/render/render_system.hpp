#pragma once

#include "render_device.hpp"

#include <memory>

namespace raytracing::renderers {

	class renderer;
	
}

namespace raytracing::runtime {

	struct runtime_service;
	struct runtime_frame;
	
}

namespace raytracing::runtime::render {

	using runtime::runtime_service;

	class render_system final : public noncopyable {
	public:
		render_system() = default;

		~render_system() = default;

		void resolve(const runtime_service& service);

		void release(const runtime_service& service);
		
		void update(const runtime_service& service, const runtime_frame& frame);

		void render(const runtime_service& service, const runtime_frame& frame);

		uint32 sample_index() const noexcept;
	private:
		std::shared_ptr<renderers::renderer> mRenderer;

		wrapper::directx12::texture2d mRenderTargetHDR;
		wrapper::directx12::texture2d mRenderTargetSDR;
	};
	
}
