#pragma once

#include "shape.hpp"

namespace path_tracing::core::shapes {

	class sphere final : public shape {
	public:
		explicit sphere(real radius, bool reverse_orientation = false);

		~sphere() = default;

		mesh_data mesh() const override;

		shape_gpu_buffer gpu_buffer() const noexcept override;
		
		real radius() const noexcept;
	private:
		real mRadius;
	};
	
}
