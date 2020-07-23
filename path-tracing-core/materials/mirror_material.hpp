#pragma once

#include "material.hpp"

namespace path_tracing::core::materials {

	class mirror_material final : public material {
	public:
		explicit mirror_material(const std::shared_ptr<texture>& reflectance);

		~mirror_material() = default;

		material_gpu_buffer gpu_buffer() const noexcept override;
	private:
		std::shared_ptr<texture> mReflectance;
	};
	
}
