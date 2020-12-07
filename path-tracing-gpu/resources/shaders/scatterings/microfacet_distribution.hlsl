#ifndef __MICROFACET_DISTRIBUTION_HLSL__
#define __MICROFACET_DISTRIBUTION_HLSL__

#include "scattering.hlsl"

float evaluate_distribution(float alpha_x, float alpha_y, float3 wh)
{
	float tan_theta_2 = tan_theta_pow2(wh);

	if (isinf(tan_theta_2)) return 0;

	float cos_theta_4 = cos_theta_pow2(wh) * cos_theta_pow2(wh);
	float e = (
		cos_phi_pow2(wh) / (alpha_x * alpha_x) +
		sin_phi_pow2(wh) / (alpha_y * alpha_y)) * tan_theta_2;

	return 1 / (ONE_PI * alpha_x * alpha_y * cos_theta_4 * (1 + e) * (1 + e));
}

float2 trowbridge_reitz_sample(float cos_theta, float u1, float u2) {
	if (cos_theta > 0.9999) {
		float r = sqrt(u1 / (1 - u1));
		float phi = TWO_PI * u2;

		return float2(r * cos(phi), r * sin(phi));
	}

	float2 slope;

	float sin_theta = sqrt(max(0, 1 - cos_theta * cos_theta));
	float tan_theta = sin_theta / cos_theta;
	float a = 1 / tan_theta;
	float g1 = 2 / (1 + sqrt(1.f + 1.f / (a * a)));

	float A = 2 * u1 / g1 - 1;

	float tmp = 1.f / (A * A - 1.f);
	if (tmp > 1e10) tmp = 1e10;

	float B = tan_theta;
	float D = sqrt(max(float(B * B * tmp * tmp - (A * A - B * B) * tmp), 0));
	float slope_x_1 = B * tmp - D;
	float slope_x_2 = B * tmp + D;

	slope.x = (A < 0 || slope_x_2 > 1.f / tan_theta) ? slope_x_1 : slope_x_2;

	float S;
	if (u2 > 0.5f) {
		S = 1.f;
		u2 = 2.f * (u2 - .5f);
	}
	else {
		S = -1.f;
		u2 = 2.f * (.5f - u2);
	}

	float z =
		(u2 * (u2 * (u2 * 0.27385f - 0.73369f) + 0.46341f)) /
		(u2 * (u2 * (u2 * 0.093073f + 0.309420f) - 1.000000f) + 0.597999f);

	slope.y = S * z * sqrt(1.f + slope.x * slope.x);

	return slope;
}

float3 trowbridge_reitz_sample(float3 wi, float alpha_x, float alpha_y, float u1, float u2) {
	float3 wi_stretched = normalize(float3(alpha_x * wi.x, alpha_y * wi.y, wi.z));

	float2 slope = trowbridge_reitz_sample(cos_theta(wi_stretched), u1, u2);

	float tmp = cos_phi(wi_stretched) * slope.x - sin_phi(wi_stretched) * slope.y;
	slope.y = sin_phi(wi_stretched) * slope.x + cos_phi(wi_stretched) * slope.y;
	slope.x = tmp;

	slope.x = alpha_x * slope.x;
	slope.y = alpha_y * slope.y;

	return normalize(float3(-slope.x, -slope.y, 1.));
}

float trowbridge_reitz_lambda(float alpha_x, float alpha_y, float3 w)
{
	float abs_tan_theta = abs(tan_theta(w));

	if (isinf(abs_tan_theta)) return 0;

	float alpha = sqrt(
		cos_phi_pow2(w) * alpha_x * alpha_x + sin_phi_pow2(w) * alpha_y * alpha_y);

	float alpha_tan_theta_2 = (alpha * abs_tan_theta) * (alpha * abs_tan_theta);

	return (-1 + sqrt(1.f + alpha_tan_theta_2)) / 2;
}

float distribution_masking_shadowing(float alpha_x, float alpha_y, float3 w)
{
	// G = 1 / (1 + lambda(w)).
	return 1 / (1 + trowbridge_reitz_lambda(alpha_x, alpha_y, w));
}

float distribution_masking_shadowing(float alpha_x, float alpha_y, float3 wo, float3 wi)
{
	// G = G(wo) * G(wi), but we use G = 1 / (1 + lambda(wo) + lambda(wi)).
	return 1 / (1 + trowbridge_reitz_lambda(alpha_x, alpha_y, wo) + trowbridge_reitz_lambda(alpha_x, alpha_y, wi));
}

float pdf_distribution(float alpha_x, float alpha_y, float3 wo, float3 wh)
{
	return evaluate_distribution(alpha_x, alpha_y, wh) *
		distribution_masking_shadowing(alpha_x, alpha_y, wo) * abs(dot(wo, wh)) / abs(cos_theta(wo));
}

float3 sample_distribution(float alpha_x, float alpha_y, float3 wo, float2 sample)
{
	bool need_flip = wo.z < 0;
	float3 wh = trowbridge_reitz_sample(need_flip ? -wo : wo, alpha_x, alpha_x, sample.x, sample.y);

	if (need_flip) return -wh; else return wh;
}

#endif