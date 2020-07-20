#include "material.hpp"

path_tracing::core::materials::material::material(const material_type& type)
	: indexable(mIndexCount++), mType(type)
{
}

path_tracing::core::materials::material_type path_tracing::core::materials::material::type() const noexcept
{
	return mType;
}

path_tracing::core::uint32 path_tracing::core::materials::material::count() noexcept
{
	return mIndexCount;
}
