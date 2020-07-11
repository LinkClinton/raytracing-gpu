#pragma once

#include "material.hpp"

namespace path_tracing::core::materials {

	class diffuse_material final : public material {
	public:
		explicit diffuse_material(const vector3& diffuse);

		~diffuse_material() = default;

		material_gpu_buffer gpu_buffer() const noexcept override;
	private:
		vector3 mDiffuse;
	};
	
}
