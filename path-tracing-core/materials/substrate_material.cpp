#include "substrate_material.hpp"

path_tracing::core::materials::substrate_material::substrate_material(
	const std::shared_ptr<texture>& specular,
	const std::shared_ptr<texture>& diffuse, 
	const std::shared_ptr<texture>& roughness_u,
	const std::shared_ptr<texture>& roughness_v,
	bool remapped_roughness_to_alpha) :
	material(material_type::substrate),
	mSpecular(specular), mDiffuse(diffuse),
	mRoughnessU(roughness_u), mRoughnessV(roughness_v),
	mRemappedRoughnessToAlpha(remapped_roughness_to_alpha)
{
}

path_tracing::core::materials::material_gpu_buffer path_tracing::core::materials::substrate_material::gpu_buffer() const noexcept
{
	material_gpu_buffer buffer;

	buffer.specular = mSpecular->index();
	buffer.diffuse = mDiffuse->index();
	buffer.roughness_u = mRoughnessU->index();
	buffer.roughness_v = mRoughnessV->index();
	buffer.remapped = mRemappedRoughnessToAlpha ? 1 : 0;
	buffer.type = type();

	return buffer;
}

std::wstring path_tracing::core::materials::substrate_material::name() const noexcept
{
	return L"substrate_material";
}
