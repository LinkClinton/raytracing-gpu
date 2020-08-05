#ifndef __SCATTERING_MICROFACET_HLSL__
#define __SCATTERING_MICROFACET_HLSL__

#include "scattering_fresnel_effect.hlsl"

float3 evaluate_conductor_microfacet_reflection(
	float3 reflectance, float3 eta, float3 k, 
	float alpha_x, float alpha_y, float3 wo, float3 wi)
{
	float cos_theta_o = abs(cos_theta(wo));
	float cos_theta_i = abs(cos_theta(wi));

	float3 wh = wi + wo;

	if (cos_theta_o == 0 || cos_theta_i == 0) return 0;

	if (wh.x == 0 && wh.y == 0 && wh.z == 0) return 0;
	
	wh = face_forward(normalize(wh), float3(0, 0, 1));

	float3 fresnel = fresnel_reflect_conductor(abs(dot(wi, wh)), 1, eta, k);
	float masking_shadowing = distribution_masking_shadowing(alpha_x, alpha_y, wo, wi);
	float distribution = evaluate_distribution(alpha_x, alpha_y, wh);

	return reflectance * (masking_shadowing * distribution) * fresnel /
		(4 * cos_theta_i * cos_theta_o);
}

float3 evaluate_dielectric_microfacet_reflection(
	float3 reflectance, float eta, float alpha_x, float alpha_y, float3 wo, float3 wi)
{
	float cos_theta_o = abs(cos_theta(wo));
	float cos_theta_i = abs(cos_theta(wi));

	float3 wh = wi + wo;
	
	if (cos_theta_o == 0 || cos_theta_i == 0) return 0;

	if (wh.x == 0 && wh.y == 0 && wh.z == 0) return 0;

	wh = face_forward(normalize(wh), float3(0, 0, 1));

	float3 fresnel = fresnel_reflect_dielectric(dot(wi, wh), 1, eta);
	float masking_shadowing = distribution_masking_shadowing(alpha_x, alpha_y, wo, wi);
	float distribution = evaluate_distribution(alpha_x, alpha_y, wh);

	return reflectance * (masking_shadowing * distribution) * fresnel /
		(4 * cos_theta_i * cos_theta_o);
}

float pdf_microfacet_reflection(float alpha_x, float alpha_y, float3 wo, float3 wi)
{
	if (!same_hemisphere(wo, wi)) return 0;

	float3 wh = normalize(wo + wi);

	return pdf_distribution(alpha_x, alpha_y, wo, wh) / (4 * dot(wo, wh));
}

scattering_sample sample_conductor_microfacet_reflection(
	float3 reflectance, float3 eta, float3 k,
	float alpha_x, float alpha_y, float3 wo, float2 value)
{
	scattering_sample sample;

	sample.type = scattering_type(scattering_reflection | scattering_glossy);
	sample.pdf = 0;
	
	if (wo.z == 0) return sample;

	float3 wh = sample_distribution(alpha_x, alpha_y, wo, value);

	if (dot(wo, wh) < 0) return sample;

	float3 wi = not_default_reflect(wo, wh);

	// if the wo and wi are not in the same hemisphere, we will return 0
	if (!same_hemisphere(wo, wi)) return sample;

	sample.value = evaluate_conductor_microfacet_reflection(reflectance, eta, k, alpha_x, alpha_y, wo, wi);
	sample.wi = wi;
	sample.pdf = pdf_distribution(alpha_x, alpha_y, wo, wh) / (4 * dot(wo, wh));

	return sample;
}

scattering_sample sample_dielectric_microfacet_reflection(
	float3 reflectance, float eta, float alpha_x, float alpha_y, float3 wo, float2 value)
{
	scattering_sample sample;

	sample.type = scattering_type(scattering_reflection | scattering_glossy);
	sample.wi = 0;
	sample.pdf = 0;

	if (wo.z == 0) return sample;

	float3 wh = sample_distribution(alpha_x, alpha_y, wo, value);

	if (dot(wo, wh) < 0) return sample;

	float3 wi = not_default_reflect(wo, wh);

	// if the wo and wi are not in the same hemisphere, we will return 0
	if (!same_hemisphere(wo, wi)) return sample;

	sample.value = evaluate_dielectric_microfacet_reflection(reflectance, eta, alpha_x, alpha_y, wo, wi);
	sample.wi = wi;
	sample.pdf = pdf_distribution(alpha_x, alpha_y, wo, wh) / (4 * dot(wo, wh));

	return sample;
}

#endif