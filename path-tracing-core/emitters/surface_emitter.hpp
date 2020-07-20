#pragma once

#include "emitter.hpp"

namespace path_tracing::core::emitters {

	class surface_emitter final : public emitter {
	public:
		explicit surface_emitter(const vector3& radiance);

		~surface_emitter() = default;

		emitter_gpu_buffer gpu_buffer(const transform& transform, uint32 index) const noexcept override;
	private:
		vector3 mRadiance;
	};
	
}
