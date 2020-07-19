#ifndef __MATERIAL_DIFFUSE_HLSL__
#define __MATERIAL_DIFFUSE_HLSL__

#include "../scatterings/scattering_lambertian.hlsl"

float3 evaluate_diffuse_material(material_gpu_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (wo.z == 0 || same_hemisphere(wo, wi) == false) return 0;

	if (has(include, scattering_type(scattering_reflection | scattering_diffuse)))
		return evaluate_lambertian_reflection(material.diffuse, wo, wi);
	
	return 0;
}

float pdf_diffuse_material(material_gpu_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (has(include, scattering_type(scattering_reflection | scattering_diffuse)))
		return pdf_lambertian_reflection(wo, wi);

	return 0;
}

scattering_sample sample_diffuse_material(material_gpu_buffer material, float3 wo, float2 value, scattering_type include = scattering_all)
{
	if (!has(include, scattering_type(scattering_reflection | scattering_diffuse)) || wo.z == 0) {
		scattering_sample sample;

		sample.type = scattering_unknown;
		sample.pdf = 0;

		return sample;
	}

	return sample_lambertian_reflection(material.diffuse, wo, value);
}

#endif