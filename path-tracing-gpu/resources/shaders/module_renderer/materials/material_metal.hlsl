#ifndef __MATERIAL_METAL_HLSL__
#define __MATERIAL_METAL_HLSL__

#include "../../scatterings/scattering_microfacet.hlsl"
#include "material.hlsl"

float3 evaluate_metal_material(material material, float3 wo, float3 wi)
{
	if (wo.z == 0 || same_hemisphere(wo, wi) == false) return 0;

	return evaluate_conductor_microfacet_reflection(1, material.eta, material.k, 
		material.roughness.x, material.roughness.y, wo, wi);
}

float pdf_metal_material(material material, float3 wo, float3 wi)
{
	return pdf_microfacet_reflection(material.roughness.x, material.roughness.y, wo, wi);
}

scattering_sample sample_metal_material(material material, float3 wo, float2 value)
{
	return sample_conductor_microfacet_reflection(1, material.eta, 
		material.k, material.roughness.x, material.roughness.y, wo, value);
}

#endif