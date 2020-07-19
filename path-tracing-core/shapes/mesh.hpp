#pragma once

#include "shape.hpp"

namespace path_tracing::core::shapes {

	class mesh final : public shape {
	public:
		explicit mesh(
			const std::vector<vector3>& positions,
			const std::vector<vector3>& normals,
			const std::vector<vector3>& uvs,
			const std::vector<unsigned>& indices,
			bool reverse_orientation = false);

		~mesh() = default;

		real area(const transform& transform) override;
	};
	
}
