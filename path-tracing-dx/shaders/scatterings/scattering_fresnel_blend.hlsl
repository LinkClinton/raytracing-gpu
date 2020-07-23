#ifndef __SCATTERING_FRESNEL_BLEND_HLSL__
#define __SCATTERING_FRESNEL_BLEND_HLSL__

#include "scattering_include.hlsl"

float pow_5(float value)
{
	return (value * value) * (value * value) * value;
}

float3 schlick_fresnel(float3 specular, float cos_theta)
{
	return specular + (float3(1, 1, 1) - specular) * pow_5(1 - cos_theta);
}

float3 evaluate_fresnel_blend_reflection(
	float3 reflection_specular, float3 reflection_diffuse, float alpha_x, float alpha_y,
	float3 wo, float3 wi)
{
	float3 wh = (wo + wi);

	if (is_black(wh)) return float3(0, 0, 0);

	wh = normalize(wh);

	// fr(wo, wi) = rd * (28 / (23 * pi)) * (1 - rs) * (1 - pow_5(1 - dot(wi, n) / 2)) * (1 - pow_5(1 - dot(wo, n) / 2))
	float3 diffuse = reflection_diffuse * (28 / (23 * ONE_PI)) *
		(float3(1, 1, 1) - reflection_specular) *
		(1 - pow_5(1 - 0.5 * abs(cos_theta(wi)))) *
		(1 - pow_5(1 - 0.5 * abs(cos_theta(wo))));

	// fr(wo, wi) = distribution * fresnel / (4 * dot(wi, wh) * max(dot(wi, n), dot(wo, n)))
	float distribution = evaluate_distribution(alpha_x, alpha_y, wh);
	float3 fresnel = schlick_fresnel(reflection_specular, dot(wo, wh));
	float3 specular = fresnel * distribution /
		(4 * abs(dot(wi, wh)) * max(abs(cos_theta(wi)), abs(cos_theta(wo))));

	return specular + diffuse;
}

float pdf_fresnel_blend_reflection(float alpha_x, float alpha_y, float3 wo, float3 wi)
{
	if (!same_hemisphere(wo, wi)) return 0;

	float3 wh = normalize(wo + wi);

	float sample_distribution_pdf = pdf_distribution(alpha_x, alpha_y, wo, wh) / (4 * dot(wo, wh));
	float sample_hemisphere_pdf = cosine_sample_hemisphere_pdf(abs(cos_theta(wi)));

	return 0.5 * (sample_distribution_pdf + sample_hemisphere_pdf);
}

scattering_sample sample_fresnel_blend_reflection(
	float3 reflection_specular, float3 reflection_diffuse, float alpha_x, float alpha_y,
	float3 wo, float2 value)
{
	scattering_sample sample;
	
	sample.type = scattering_type(scattering_reflection | scattering_glossy);
	sample.pdf = 0;
	
	if (value.x < 0.5) {

		float2 sample_remapped = float2(min(2 * value.x, 1 - EPSILON), value.y);
		float3 wi = cosine_sample_hemisphere(sample_remapped);

		if (wo.z < 0) wi.z *= -1;

		sample.value = evaluate_fresnel_blend_reflection(reflection_specular, reflection_diffuse, alpha_x, alpha_y, wo, wi);
		sample.wi = wi;
		sample.pdf = pdf_fresnel_blend_reflection(alpha_x, alpha_y, wo, wi);

		return sample;
	}
	
	float2 sample_remapped = float2(min(2 * (value.x - 0.5), 1 - EPSILON), value.y);
	float3 wh = sample_distribution(alpha_x, alpha_y, wo, sample_remapped);
	float3 wi = reflect(wo, wh);

	if (!same_hemisphere(wo, wi)) return sample;

	sample.value = evaluate_fresnel_blend_reflection(reflection_specular, reflection_diffuse, alpha_x, alpha_y, wo, wi);
	sample.wi = wi;
	sample.pdf = pdf_fresnel_blend_reflection(alpha_x, alpha_y, wo, wi);

	return sample;
}

#endif