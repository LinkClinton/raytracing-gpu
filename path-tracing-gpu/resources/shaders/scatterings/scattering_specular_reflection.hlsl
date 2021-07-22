#ifndef __SCATTERING_SPECULAR_REFLECTION__
#define __SCATTERING_SPECULAR_REFLECTION__

#include "scattering_fresnel_effect.hlsl"
#include "scattering.hlsl"

float3 evaluate_specular_reflection(float3 reflectance, float3 wo, float3 wi)
{
	return 0;
}

scattering_sample sample_specular_reflection(float3 reflectance, float3 wo, float eta_i, float eta_o, float2 value)
{
	const float3 wi = float3(-wo.x, -wo.y, wo.z);

	scattering_sample sample;

	sample.type = scattering_type(scattering_reflection | scattering_specular);
	sample.value = reflectance * fresnel_reflect_dielectric(cos_theta(wi), eta_i, eta_o) / abs(cos_theta(wi));
	sample.wi = wi;
	sample.pdf = 1;

	return sample;
}

float pdf_specular_reflection(float3 wo, float3 wi)
{
	return 0;
}

#endif