#ifndef __SAMPLING_HLSL__
#define __SAMPLING_HLSL__

#include "../types.hlsl"

struct uniform_hemisphere
{
	
};

template <typename Distribution>
struct sampling_method
{
};

template <>
struct sampling_method<uniform_hemisphere>
{
    static float3 get(float2 v);
    static float pdf();
};

typedef sampling_method< uniform_hemisphere> sampling_uniform_hemisphere;

float3 sampling_method<uniform_hemisphere>::get(float2 v)
{
	float r = sqrt(1 - v.x * v.x);
	float phi = 2 * ONE_PI * v.y;

    return float3(r * cos(phi), r * sin(phi), v.x);
}

float sampling_method<uniform_hemisphere>::pdf()
{
	return ONE_OVER_TWO_PI;
}

#endif