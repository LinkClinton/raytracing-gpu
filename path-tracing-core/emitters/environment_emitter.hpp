#pragma once

#include "../textures/texture.hpp"
#include "emitter.hpp"

namespace path_tracing::core::emitters {

	using namespace textures;
	
	class environment_emitter final : public emitter {
	public:
		explicit environment_emitter(const std::shared_ptr<texture>& map, const vector3& intensity);

		explicit environment_emitter(const vector3& intensity);

		emitter_gpu_buffer gpu_buffer(const transform& transform, uint32 index) const noexcept override;
	private:
		std::shared_ptr<texture> mMap;
		vector3 mIntensity;
	};
	
}
