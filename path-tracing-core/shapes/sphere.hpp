#pragma once

#include "shape.hpp"

namespace path_tracing::core::shapes {

	class sphere final : public shape {
	public:
		explicit sphere(real radius, bool reverse_orientation = false);

		~sphere() = default;

		real area(const transform& transform) override;
		
		real radius() const noexcept;
	private:
		real mRadius;
	};
	
}
