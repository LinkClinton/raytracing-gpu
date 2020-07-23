#pragma once

#include "../interfaces/noncopyable.hpp"
#include "../interfaces/indexable.hpp"
#include "../shared/transform.hpp"

#include <vector>

namespace path_tracing::core::shapes {

	using namespace shared;

	struct shape_gpu_buffer {
		uint32 positions = 0;
		uint32 normals = 0;
		uint32 indices = 0;
		uint32 uvs = 0;

		uint32 reverse = 0;
		vector3 unused = vector3(0);
		
		shape_gpu_buffer() = default;

		shape_gpu_buffer(uint32 positions, uint32 normals, uint32 indices, uint32 uvs);
	};
	
	class shape : public interfaces::noncopyable, public interfaces::indexable {
	public:
		explicit shape(bool reverse_orientation);

		~shape() = default;
		
		virtual real area(const transform& transform) = 0;

		virtual shape_gpu_buffer gpu_buffer() const noexcept;

		const std::vector<vector3>& positions() const noexcept;

		const std::vector<vector3>& normals() const noexcept;

		const std::vector<vector3>& uvs() const noexcept;

		const std::vector<unsigned>& indices() const noexcept;

		bool reverse_orientation() const noexcept;

		static uint32 count() noexcept;
	protected:
		bool mReverseOrientation = false;

		std::vector<vector3> mPositions;
		std::vector<vector3> mNormals;
		std::vector<vector3> mUvs;

		std::vector<unsigned> mIndices;
	private:
		static inline uint32 mIndexCount = 0;
	};	
}
