#ifndef __MATERIAL_GLASS_HLSL__
#define __MATERIAL_GLASS_HLSL__

#include "../../scatterings/scattering_fresnel_specular.hlsl"
#include "material.hlsl"

float3 evaluate_glass_material(material material, float3 wo, float3 wi)
{
	return evaluate_fresnel_specular();
}

float pdf_glass_material(material material, float3 wo, float3 wi)
{
	return pdf_fresnel_specular();
}

scattering_sample sample_glass_material(material material, float3 wo, float2 value)
{
	return sample_fresnel_specular(material.transmission, material.reflectance, 1, material.eta.x, wo, value);
}

#endif