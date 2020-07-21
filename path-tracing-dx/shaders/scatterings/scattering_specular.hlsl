#ifndef __SCATTERING_SPECULAR_HLSL__
#define __SCATTERING_SPECULAR_HLSL__

#include "scattering_include.hlsl"

float3 evaluate_specular_reflection(float3 reflectance, float3 wo, float3 wi)
{
	return 0;
}

float pdf_specular_reflection(float3 wo, float3 wi)
{
	return 0;
}

scattering_sample sample_specular_reflection(float3 reflectance, float3 wo, float2 value)
{
	scattering_sample sample;
	
	float3 wi = float3(-wo.x, -wo.y, wo.z);

	// todo : support fresnel effect
	sample.type = scattering_type(scattering_reflection | scattering_specular);
	sample.value = reflectance / abs(cos_theta(wi));
	sample.wi = wi;
	sample.pdf = 1;

	return sample;
}

#endif