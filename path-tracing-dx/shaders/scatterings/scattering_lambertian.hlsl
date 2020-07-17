#ifndef __SCATTERING_LAMBERTIAN_HLSL__
#define __SCATTERING_LAMBERTIAN_HLSL__

#include "scattering_include.hlsl"

float3 evaluate_lambertian_reflection(float3 reflectance, float3 wo, float3 wi)
{
	return reflectance * ONE_OVER_PI;
}

float pdf_lambertian_reflection(float3 wo, float3 wi)
{
	return same_hemisphere(wo, wi) ? cosine_sample_hemisphere_pdf(abs(cos_theta(wi))) : 0;
}

scattering_sample sample_lambertian_reflection(float3 reflectance, float3 wo, float2 value)
{
	float3 wi = cosine_sample_hemisphere(value);

	if (wo.z < 0) wi.z *= -1;

	scattering_sample sample;

	sample.type = scattering_reflection | scattering_diffuse;
	sample.value = evaluate_lambertian_reflection(reflectance, wo, wi);
	sample.wi = wi;
	sample.pdf = 1;

	return sample;
}

#endif