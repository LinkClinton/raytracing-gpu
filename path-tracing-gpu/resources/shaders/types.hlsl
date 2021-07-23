#ifndef __TYPES_HLSL__
#define __TYPES_HLSL__

#include "fix_resharper_error.hlsl"

typedef BuiltInTriangleIntersectionAttributes HitAttributes;

typedef float real;
typedef float2 vector2;
typedef float3 vector3;
typedef float4 vector4;

typedef RayDesc ray_desc;

#define ONE_OVER_PI 0.318309886183790671537767526745028724
#define ONE_OVER_TWO_PI 0.159154943091895335768883763372514362
#define QUARTER_PI 0.785398163397448309615660845819875721
#define HALF_PI 1.57079632679489661923132169163975144
#define TWO_PI 6.28318530717958647692528676655900576
#define ONE_PI 3.14159265358979323846264338327950288

#define RAY_INFINITY 1e20
#define RAY_EPSILON 8.940697e-05

#define SHADOW_EPSILON 0.0008940697
#define SHADOW_FLAG RAY_FLAG_FORCE_OPAQUE | RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER

#define EPSILON 1.192092896e-07F

#define INDEX_NULL 0xffffffff

float2 concentric_sample_disk(float2 sample)
{
	const float2 sample_remapped = 2 * sample.xy - 1;

	if (sample_remapped.x == 0 && sample_remapped.y == 0) return float2(0, 0);

	float theta;
	float r;

	if (abs(sample_remapped.x) > abs(sample_remapped.y)) {
		r = sample_remapped.x;
		theta = QUARTER_PI * (sample_remapped.y / sample_remapped.x);
	}
	else {
		r = sample_remapped.y;
		theta = HALF_PI - QUARTER_PI * (sample_remapped.x / sample_remapped.y);
	}

	return float2(cos(theta), sin(theta)) * r;
}

float3 cosine_sample_hemisphere(float2 sample)
{
	const float2 disk = concentric_sample_disk(sample);
	const float z = sqrt(max(0, 1 - dot(disk, disk)));

	return float3(disk.x, disk.y, z);
}

float cosine_sample_hemisphere_pdf(float cos_theta)
{
	return cos_theta * ONE_OVER_PI;
}

float2 uniform_sample_triangle(float2 value)
{
	float u = sqrt(value.x);

	return float2(1 - u, value.y * u);
}

float3 spherical_direction(float sin_theta, float cos_theta, float phi)
{
	return float3(
		sin_theta * cos(phi),
		sin_theta * sin(phi),
		cos_theta
		);
}

float spherical_theta(float3 v)
{
	return acos(clamp(v.z, -1, 1));
}

float spherical_phi(float3 v)
{
	float phi = atan2(v.y, v.x);

	return phi < 0 ? phi + TWO_PI : phi;
}


float gamma_correct(float value)
{
	if (value <= 0.0031308f) return 12.92f * value;

	return 1.055f * pow(value, 1.f / 2.4f) - 0.055f;
}

float3 gamma_correct(float3 value)
{
	return float3(
		gamma_correct(value.x),
		gamma_correct(value.y),
		gamma_correct(value.z));
}

float max_component(float3 value)
{
	if (value.x > value.y && value.x > value.z) return value.x;
	if (value.y > value.z) return value.y;
	return value.z;
}

float distance_squared(float3 v0, float3 v1)
{
	return dot(v0 - v1, v0 - v1);
}

float length_squared(float3 v)
{
	return distance_squared(v, float3(0, 0, 0));
}

bool is_black(float3 value)
{
	return value.x == 0 && value.y == 0 && value.z == 0;
}

float power_heuristic(float f_pdf, float g_pdf)
{
	return (f_pdf * f_pdf) / (f_pdf * f_pdf + g_pdf * g_pdf);
}

float3 face_forward(float3 v, float3 forward)
{
	return dot(v, forward) > 0 ? v : -v;
}

#endif