#include "metal_material.hpp"

path_tracing::core::materials::metal_material::metal_material(
	const std::shared_ptr<texture>& eta,
	const std::shared_ptr<texture>& k, 
	const std::shared_ptr<texture>& roughness_u,
	const std::shared_ptr<texture>& roughness_v, 
	bool remapped_roughness_to_alpha) :
	material(material_type::metal), mEta(eta), mK(k), mRoughnessU(roughness_u), mRoughnessV(roughness_v),
	mRemappedRoughnessToAlpha(remapped_roughness_to_alpha)
{
}

path_tracing::core::materials::material_gpu_buffer path_tracing::core::materials::metal_material::gpu_buffer() const noexcept
{
	material_gpu_buffer buffer;

	buffer.eta = mEta->index();
	buffer.k = mK->index();
	buffer.roughness_u = mRoughnessU->index();
	buffer.roughness_v = mRoughnessV->index();
	buffer.remapped = mRemappedRoughnessToAlpha;
	buffer.type = type();

	return buffer;
}

std::wstring path_tracing::core::materials::metal_material::name() const noexcept
{
	return L"metal_material";
}
