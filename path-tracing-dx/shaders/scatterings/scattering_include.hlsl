#ifndef __SCATTERING_INCLUDE_HLSL__
#define __SCATTERING_INCLUDE_HLSL__

#include "../utility.hlsl"

enum scattering_type : uint {
	scattering_unknown = 0,
	scattering_reflection = 1 << 0,
	scattering_transmission = 1 << 1,
	scattering_specular = 1 << 2,
	scattering_diffuse = 1 << 3,
	scattering_glossy = 1 << 4,
	scattering_all = scattering_reflection | scattering_transmission | scattering_specular | scattering_diffuse | scattering_glossy
};

struct scattering_sample {
	scattering_type type;
	float3 value;
	float3 wi;
	float pdf;
};

bool has(scattering_type lhs, scattering_type rhs)
{
	return (lhs & rhs) == rhs;
}

float cos_theta(float3 w)
{
	return w.z;
}

float cos_theta_pow2(float3 w)
{
	return cos_theta(w) * cos_theta(w);
}

float sin_theta_pow2(float3 w)
{
	return max(0, 1 - cos_theta_pow2(w));
}

float tan_theta_pow2(float3 w)
{
	return sin_theta_pow2(w) / cos_theta_pow2(w);
}

float sin_theta(float3 w)
{
	// sin^2 theta + cos^2 theta = 1
	// so sin_theta = sqrt(1 - cos_theta_pow2)
	return sqrt(sin_theta_pow2(w));
}

float tan_theta(float3 w)
{
	return sin_theta(w) / cos_theta(w);
}

float cos_phi(float3 w)
{
	float sin = sin_theta(w);

	return sin == 0 ? 1 : clamp(w.x / sin, -1, 1);
}

float sin_phi(float3 w)
{
	float sin = sin_theta(w);

	return sin == 0 ? 0 : clamp(w.y / sin, -1, 1);
}

float cos_phi_pow2(float3 w)
{
	return cos_phi(w) * cos_phi(w);
}

float sin_phi_pow2(float3 w)
{
	return sin_phi(w) * sin_phi(w);
}

bool same_hemisphere(float3 v0, float3 v1)
{
	return v0.z * v1.z > 0;
}

float3 not_default_reflect(float3 wi, float3 normal)
{
	return -wi + 2 * dot(wi, normal) * normal;
}

float roughness_to_alpha(float roughness)
{
	roughness = max(roughness, 1e-3);

	float x = log(roughness);

	return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x +
		0.000640711f * x * x * x * x;
}

#include "microfacet_distribution.hlsl"

#endif