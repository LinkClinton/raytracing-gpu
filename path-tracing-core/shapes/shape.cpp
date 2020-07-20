#include "shape.hpp"

path_tracing::core::shapes::shape_gpu_buffer::shape_gpu_buffer(uint32 positions, uint32 normals, uint32 indices, uint32 uvs) :
	positions(positions), normals(normals), indices(indices), uvs(uvs)
{
}

path_tracing::core::shapes::shape::shape(bool reverse_orientation) :
	indexable(mIndexCount++), mReverseOrientation(reverse_orientation)
{
}

path_tracing::core::shapes::shape_gpu_buffer path_tracing::core::shapes::shape::gpu_buffer() const noexcept
{
	shape_gpu_buffer buffer;

	buffer.positions = static_cast<uint32>(mPositions.size());
	buffer.normals = static_cast<uint32>(mNormals.size());
	buffer.uvs = static_cast<uint32>(mUvs.size());

	buffer.indices = static_cast<uint32>(mIndices.size() / 3);

	return buffer;
}

const std::vector<path_tracing::core::shared::vector3>& path_tracing::core::shapes::shape::positions() const noexcept
{
	return mPositions;
}

const std::vector<path_tracing::core::shared::vector3>& path_tracing::core::shapes::shape::normals() const noexcept
{
	return mNormals;
}

const std::vector<path_tracing::core::shared::vector3>& path_tracing::core::shapes::shape::uvs() const noexcept
{
	return mUvs;
}

const std::vector<unsigned>& path_tracing::core::shapes::shape::indices() const noexcept
{
	return mIndices;
}

bool path_tracing::core::shapes::shape::reverse_orientation() const noexcept
{
	return mReverseOrientation;
}

path_tracing::core::uint32 path_tracing::core::shapes::shape::count() noexcept
{
	return mIndexCount;
}
