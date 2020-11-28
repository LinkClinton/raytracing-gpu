#ifndef __MATERIAL_DIFFUSE_HLSL__
#define __MATERIAL_DIFFUSE_HLSL__

#include "../../scatterings/scattering_lambertian.hlsl"

#include "material.hlsl"

float3 evaluate_diffuse_material(material material, float3 wo, float3 wi)
{
	if (wo.z == 0 || same_hemisphere(wo, wi) == false) return 0;

	return evaluate_lambertian_reflection(material.diffuse, wo, wi);
}

float pdf_diffuse_material(material material, float3 wo, float3 wi)
{
	return pdf_lambertian_reflection(wo, wi);
}

scattering_sample sample_diffuse_material(material material, float3 wo, float2 value)
{
	return sample_lambertian_reflection(material.diffuse, wo, value);
}

#endif