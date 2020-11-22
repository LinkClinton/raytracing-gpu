#pragma once

#include "../runtime/runtime_service.hpp"
#include "../runtime/runtime_frame.hpp"

namespace path_tracing::renderers {

	using runtime::runtime_service;
	using runtime::runtime_frame;
	
	class renderer : public noncopyable {
	public:
		renderer(const runtime_service& service);

		~renderer() = default;

		virtual void update(const runtime_service& service, const runtime_frame& frame) = 0;

		virtual void render(const runtime_service& service, const runtime_frame& frame) = 0;
	};
	
}
