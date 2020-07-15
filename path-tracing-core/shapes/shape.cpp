#include "shape.hpp"

path_tracing::core::shapes::mesh_data::mesh_data(
	const std::vector<vector3>& positions,
	const std::vector<vector3>& normals,
	const std::vector<vector3>& uvs, 
	const std::vector<unsigned>& indices) :
	positions(positions), normals(normals), uvs(uvs), indices(indices)
{
}

path_tracing::core::shapes::shape::shape(bool reverse_orientation) :
	mReverseOrientation(reverse_orientation)
{
}

bool path_tracing::core::shapes::shape::reverse_orientation() const noexcept
{
	return mReverseOrientation;
}
