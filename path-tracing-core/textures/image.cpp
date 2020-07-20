#include "image.hpp"

path_tracing::core::textures::image::image(const std::vector<shared::vector3>& values, size_t width, size_t height) :
	indexable(mIndexCount++), mVector3Values(values), mWidth(width), mHeight(height)
{
}

path_tracing::core::textures::image::image(const std::vector<shared::vector2>& values, size_t width, size_t height) :
	indexable(mIndexCount++), mVector2Values(values), mWidth(width), mHeight(height)
{
}

path_tracing::core::textures::image::image(const std::vector<core::real>& values, size_t width, size_t height) :
	indexable(mIndexCount++), mRealValues(values), mWidth(width), mHeight(height)
{
}

const std::vector<path_tracing::core::shared::vector3>& path_tracing::core::textures::image::vector3() const noexcept
{
	return mVector3Values;
}

const std::vector<path_tracing::core::shared::vector2>& path_tracing::core::textures::image::vector2() const noexcept
{
	return mVector2Values;
}

const std::vector<path_tracing::core::real>& path_tracing::core::textures::image::real() const noexcept
{
	return mRealValues;
}

size_t path_tracing::core::textures::image::width() const noexcept
{
	return mWidth;
}

size_t path_tracing::core::textures::image::height() const noexcept
{
	return mHeight;
}

path_tracing::core::uint32 path_tracing::core::textures::image::count() noexcept
{
	return mIndexCount;
}
