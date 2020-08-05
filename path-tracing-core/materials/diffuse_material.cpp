#include "diffuse_material.hpp"

path_tracing::core::materials::diffuse_material::diffuse_material(const std::shared_ptr<texture>& diffuse) :
	material(material_type::diffuse), mDiffuse(diffuse)
{
}

path_tracing::core::materials::material_gpu_buffer path_tracing::core::materials::diffuse_material::gpu_buffer() const noexcept
{
	material_gpu_buffer buffer;

	buffer.diffuse = mDiffuse->index();
	buffer.type = type();

	return buffer;
}

std::wstring path_tracing::core::materials::diffuse_material::name() const noexcept
{
	return L"diffuse_material";
}
