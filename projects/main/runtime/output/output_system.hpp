#pragma once

#include "../../interfaces/noncopyable.hpp"
#include "../../types.hpp"

namespace raytracing::runtime {

	struct runtime_service;
	struct runtime_frame;
	
}

namespace raytracing::runtime::output {

	class output_system final : public noncopyable {
	public:
		output_system() = default;

		~output_system() = default;

		void resolve(const runtime_service& service);

		void release(const runtime_service& service);
		
		void update(const runtime_service& service, const runtime_frame& frame);
	private:
		real mTotalTime = 0;
	};
	
}
