#pragma once

#include "emitter.hpp"

namespace path_tracing::core::emitters {

	class point_emitter final : public emitter {
	public:
		explicit point_emitter(const vector3& intensity);

		~point_emitter() = default;

		emitter_gpu_buffer gpu_buffer(const transform& transform) const noexcept override;
	private:
		vector3 mIntensity;
	};
	
}
