#pragma once

#include "renderer.hpp"

namespace path_tracing::renderers {

	struct submodule final {
		std::wstring filename = L"";

		submodule() = default;
	};

	class module_renderer : public renderer {
	public:
		module_renderer(
			const std::vector<submodule>& materials,
			const std::vector<submodule>& lights,
			const runtime_service& service);

		~module_renderer() = default;

		void update(const runtime_service& service, const runtime_frame& frame) override;

		void render(const runtime_service& service, const runtime_frame& frame) override;
	private:
		
	};
	
}
