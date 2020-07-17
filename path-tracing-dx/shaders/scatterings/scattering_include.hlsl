#ifndef __SCATTERING_INCLUDE_HLSL__
#define __SCATTERING_INCLUDE_HLSL__

#include "../utility.hlsl"

enum scattering_type {
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

bool same_hemisphere(float3 v0, float3 v1)
{
	return v0.z * v1.z > 0;
}

#endif