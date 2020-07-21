#include "mirror_material.hpp"

path_tracing::core::materials::mirror_material::mirror_material(const vector3& reflectance) : material(material_type::mirror), mReflectance(reflectance)
{
}

path_tracing::core::materials::material_gpu_buffer path_tracing::core::materials::mirror_material::gpu_buffer() const noexcept
{
	material_gpu_buffer buffer;

	buffer.reflectance = mReflectance;
	buffer.type = type();

	return buffer;
}
