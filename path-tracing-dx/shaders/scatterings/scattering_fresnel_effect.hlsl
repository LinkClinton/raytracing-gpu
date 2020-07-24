#ifndef __SCATTERING_FRESNEL_EFFECT_HLSL__
#define __SCATTERING_FRESNEL_EFFECT_HLSL__

#include "scattering_include.hlsl"

float3 fresnel_reflect_conductor(float cos_theta_i, float3 eta_i, float3 eta_o, float3 k)
{
	cos_theta_i = clamp(cos_theta_i, -1, 1);

	float3 eta = eta_o / eta_i;
	float3 eta_k = k / eta_i;

	float cos_theta_i_pow2 = cos_theta_i * cos_theta_i;
	float sin_theta_i_pow2 = 1 - cos_theta_i_pow2;

	float3 eta_pow2 = eta * eta;
	float3 eta_k_pow2 = eta_k * eta_k;

	float3 t0 = eta_pow2 - eta_k_pow2 - sin_theta_i_pow2;
	float3 a_pow2_plus_b_pow2 = sqrt(t0 * t0 + eta_pow2 * eta_k_pow2 * 4);
	float3 t1 = a_pow2_plus_b_pow2 + cos_theta_i_pow2;
	float3 a = sqrt((a_pow2_plus_b_pow2 + t0) * 0.5);
	float3 t2 = a * 2 * cos_theta_i;
	float3 t3 = a_pow2_plus_b_pow2 * cos_theta_i_pow2 + sin_theta_i_pow2 * sin_theta_i_pow2;
	float3 t4 = t2 * sin_theta_i_pow2;

	float3 r0 = (t1 - t2) / (t1 + t2);
	float3 r1 = r0 * (t3 - t4) / (t3 + t4);

	return (r0 + r1) * 0.5;
}

#endif