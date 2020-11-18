#pragma once

#include "../runtime/runtime_service.hpp"

namespace path_tracing::renderers {

	using runtime::runtime_service;
	
	class renderer : public noncopyable {
	public:
		renderer(const runtime_service& service);

		~renderer() = default;

		virtual void update(const runtime_service& service, real delta) = 0;

		virtual void render(const runtime_service& service, real delta) = 0;
	};
	
}
