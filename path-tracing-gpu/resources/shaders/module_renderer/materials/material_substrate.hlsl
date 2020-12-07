#ifndef __MATERIAL_SUBSTRATE_HLSL__
#define __MATERIAL_SUBSTRATE_HLSL__

#include "../../scatterings/scattering_fresnel_blend.hlsl"
#include "material.hlsl"

float3 evaluate_substrate_material(material material, float3 wo, float3 wi)
{
	if (wo.z == 0 || same_hemisphere(wo, wi) == false) return 0;

	return evaluate_fresnel_blend_reflection(
		material.specular, material.diffuse, material.roughness.x, material.roughness.y, wo, wi);
}

float pdf_substrate_material(material material, float3 wo, float3 wi)
{
	return pdf_fresnel_blend_reflection(material.roughness.x, material.roughness.y, wo, wi);
}

scattering_sample sample_substrate_material(material material, float3 wo, float2 value)
{
	return sample_fresnel_blend_reflection(material.specular, material.diffuse, 
		material.roughness.x, material.roughness.y, wo, value);
}

#endif