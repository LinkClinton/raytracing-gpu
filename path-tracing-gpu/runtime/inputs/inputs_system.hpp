#pragma once

#include "../../interfaces/noncopyable.hpp"
#include "../../types.hpp"

namespace path_tracing::runtime {

	struct runtime_service;
	struct runtime_frame;
	
}

namespace path_tracing::runtime::inputs {

	class inputs_system final : public noncopyable {
	public:
		inputs_system() = default;

		~inputs_system() = default;

		void resolve(const runtime_service& service);

		void release(const runtime_service& service);

		void update(const runtime_service& service, const runtime_frame& frame);
	private:
		vector2 mLastMousePosition = vector2(-1);
	};
	
}
