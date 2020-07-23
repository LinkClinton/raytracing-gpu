#pragma once

#include "material.hpp"

namespace path_tracing::core::materials {

	class substrate_material final : public material {
	public:
		explicit substrate_material(
			const std::shared_ptr<texture>& specular,
			const std::shared_ptr<texture>& diffuse,
			const std::shared_ptr<texture>& roughness_u,
			const std::shared_ptr<texture>& roughness_v,
			bool remapped_roughness_to_alpha = false);

		~substrate_material() = default;

		material_gpu_buffer gpu_buffer() const noexcept override;
	private:
		std::shared_ptr<texture> mSpecular;
		std::shared_ptr<texture> mDiffuse;
		std::shared_ptr<texture> mRoughnessU;
		std::shared_ptr<texture> mRoughnessV;

		bool mRemappedRoughnessToAlpha = false;
	};
	
}
