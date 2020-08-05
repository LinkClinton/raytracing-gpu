#include "mirror_material.hpp"

path_tracing::core::materials::mirror_material::mirror_material(const std::shared_ptr<texture>& reflectance) :
	material(material_type::mirror), mReflectance(reflectance)
{
}

path_tracing::core::materials::material_gpu_buffer path_tracing::core::materials::mirror_material::gpu_buffer() const noexcept
{
	material_gpu_buffer buffer;

	buffer.reflectance = mReflectance->index();
	buffer.type = type();

	return buffer;
}

std::wstring path_tracing::core::materials::mirror_material::name() const noexcept
{
	return L"mirror_material";
}
