#include "mesh.hpp"

#include <array>

path_tracing::core::shapes::mesh::mesh(
	const std::vector<vector3>& positions, 
	const std::vector<vector3>& normals,
	const std::vector<vector3>& uvs,
	const std::vector<unsigned>& indices,
	bool reverse_orientation) : shape(reverse_orientation)
{
	mPositions = positions;
	mNormals = normals;
	mIndices = indices;
	mUvs = uvs;
}

path_tracing::core::real path_tracing::core::shapes::mesh::area(const transform& transform)
{
	real area = 0;
	
	for (size_t index = 0; index < mIndices.size(); index+= 3) {
		std::array<vector3, 3> points = {
			mPositions[mIndices[index + 0]],
			mPositions[mIndices[index + 1]],
			mPositions[mIndices[index + 2]]
		};

		points[0] = transform_point(transform, points[0]);
		points[1] = transform_point(transform, points[1]);
		points[2] = transform_point(transform, points[2]);

		area += static_cast<real>(0.5) * length(cross(points[2] - points[0], points[2] - points[1]));
	}

	return area;
}
