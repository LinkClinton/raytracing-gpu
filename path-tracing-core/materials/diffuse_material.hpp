#pragma once

#include "material.hpp"

namespace path_tracing::core::materials {

	class diffuse_material final : public material {
	public:
		explicit diffuse_material(const std::shared_ptr<texture>& diffuse);

		~diffuse_material() = default;

		material_gpu_buffer gpu_buffer() const noexcept override;

		std::wstring name() const noexcept override;
	private:
		std::shared_ptr<texture> mDiffuse;
	};
	
}
