#include "material.hpp"

path_tracing::core::materials::material::material(const material_type& type) : mType(type)
{
}

path_tracing::core::materials::material_type path_tracing::core::materials::material::type() const noexcept
{
	return mType;
}
