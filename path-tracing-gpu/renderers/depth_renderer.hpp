#pragma once

#include "renderer.hpp"

namespace path_tracing::renderers {

	class depth_renderer final : public renderer {
	public:
		depth_renderer(const runtime_service& service);

		~depth_renderer() = default;

		void update(const runtime_service& service, real delta) override;

		void render(const runtime_service& service, real delta) override;
	private:
		
	};
	
}
