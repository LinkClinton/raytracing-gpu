#ifndef __SCATTERING_LAMBERTIAN_HLSL__
#define __SCATTERING_LAMBERTIAN_HLSL__

#include "scattering.hlsl"

float3 evaluate_lambertian_reflection(float3 diffuse, float3 wo, float3 wi)
{
	return diffuse * ONE_OVER_PI;
}

float pdf_lambertian_reflection(float3 wo, float3 wi)
{
	return same_hemisphere(wo, wi) ? cosine_sample_hemisphere_pdf(abs(cos_theta(wi))) : 0;
}

scattering_sample sample_lambertian_reflection(float3 diffuse, float3 wo, float2 value)
{
	float3 wi = cosine_sample_hemisphere(value);

	if (wo.z < 0) wi.z *= -1;

	scattering_sample sample;

	sample.type = scattering_type(scattering_reflection | scattering_diffuse);
	sample.value = evaluate_lambertian_reflection(diffuse, wo, wi);
	sample.wi = wi;
	sample.pdf = pdf_lambertian_reflection(wo, wi);

	return sample;
}

#endif