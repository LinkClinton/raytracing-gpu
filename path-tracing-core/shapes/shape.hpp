#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../shared/transform.hpp"

#include <vector>

namespace path_tracing::core::shapes {

	using namespace shared;
	
	struct mesh_data {
		std::vector<vector3> positions;
		std::vector<vector3> normals;
		std::vector<vector3> uvs;

		std::vector<unsigned> indices;

		mesh_data() = default;

		mesh_data(
			const std::vector<vector3>& positions,
			const std::vector<vector3>& normals,
			const std::vector<vector3>& uvs,
			const std::vector<unsigned>& indices);
	};

	struct shape_gpu_buffer {
		uint32 positions = 0;
		uint32 normals = 0;
		uint32 indices = 0;
		uint32 uvs = 0;

		shape_gpu_buffer() = default;

		shape_gpu_buffer(uint32 positions, uint32 normals, uint32 indices, uint32 uvs);
	};
	
	class shape : public interfaces::noncopyable {
	public:
		explicit shape(bool reverse_orientation);

		~shape() = default;

		virtual mesh_data mesh() const = 0;

		virtual real area(const transform& transform) = 0;

		virtual shape_gpu_buffer gpu_buffer() const noexcept = 0;
		
		bool reverse_orientation() const noexcept;
	private:
		bool mReverseOrientation = false;
	};	
}
