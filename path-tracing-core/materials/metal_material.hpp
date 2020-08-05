#pragma once

#include "material.hpp"

namespace path_tracing::core::materials {

	class metal_material final : public material {
	public:
		explicit metal_material(
			const std::shared_ptr<texture>& eta,
			const std::shared_ptr<texture>& k,
			const std::shared_ptr<texture>& roughness_u,
			const std::shared_ptr<texture>& roughness_v,
			bool remapped_roughness_to_alpha = false);

		~metal_material() = default;

		material_gpu_buffer gpu_buffer() const noexcept override;

		std::wstring name() const noexcept override;
	private:
		std::shared_ptr<texture> mEta;
		std::shared_ptr<texture> mK;

		std::shared_ptr<texture> mRoughnessU;
		std::shared_ptr<texture> mRoughnessV;

		bool mRemappedRoughnessToAlpha = false;
	};
	
}
