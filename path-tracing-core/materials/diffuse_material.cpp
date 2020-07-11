#include "diffuse_material.hpp"

path_tracing::core::materials::diffuse_material::diffuse_material(const vector3& diffuse) :
	material(material_type::diffuse), mDiffuse(diffuse)
{
}

path_tracing::core::materials::material_gpu_buffer path_tracing::core::materials::diffuse_material::gpu_buffer() const noexcept
{
	material_gpu_buffer buffer;

	buffer.diffuse = mDiffuse;
	buffer.type = type();

	return buffer;
}
