#pragma once

#include "material.hpp"

namespace path_tracing::core::materials {

	class uber_material final : public material {
	public:
		explicit uber_material(
			const std::shared_ptr<texture>& specular,
			const std::shared_ptr<texture>& diffuse,
			const std::shared_ptr<texture>& eta,
			const std::shared_ptr<texture>& roughness_u,
			const std::shared_ptr<texture>& roughness_v,
			bool remapped_roughness_to_alpha = true);

		~uber_material() = default;

		material_gpu_buffer gpu_buffer() const noexcept override;

		std::wstring name() const noexcept override;
	private:
		std::shared_ptr<texture> mSpecular;
		std::shared_ptr<texture> mDiffuse;
		std::shared_ptr<texture> mEta;
		std::shared_ptr<texture> mRoughnessU;
		std::shared_ptr<texture> mRoughnessV;

		bool mRemappedRoughnessToAlpha = false;
	};
	
}
