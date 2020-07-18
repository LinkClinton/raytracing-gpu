#ifndef __MATERIAL_HLSL__
#define __MATERIAL_HLSL__

#include "material_include.hlsl"
#include "material_diffuse.hlsl"

float3 evaluate_material(material_gpu_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (material.type == material_diffuse)
		return evaluate_diffuse_material(material, wo, wi, include);

	return 0;
}

float pdf_material(material_gpu_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (material.type == material_diffuse)
		return pdf_diffuse_material(material, wo, wi, include);

	return 0;
}

scattering_sample sample_material(material_gpu_buffer material, float3 wo, float2 value, scattering_type include = scattering_all)
{
	if (material.type == material_diffuse)
		return sample_diffuse_material(material, wo, value, include);

	scattering_sample sample; sample.pdf = 0; sample.type = scattering_unknown;

	return sample;
}

#endif