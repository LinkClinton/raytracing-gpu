#ifndef __LIGHT_ENVIRONMENT_HLSL__
#define __LIGHT_ENVIRONMENT_HLSL__

#include "../module_types.hlsl"
#include "light.hlsl"

float3 evaluate_environment_light(light light, interaction interaction, float3 wi)
{
#ifdef __ENABLE_ENVIRONMENT_IMAGE__
	float3 w = normalize(-mul(float4(wi, 0), entities[light.entity].world_to_local).xyz);

	float2 value = float2(spherical_phi(w) * ONE_OVER_TWO_PI, spherical_theta(w) * ONE_OVER_PI);

	return light.intensity * environment.SampleLevel(default_sampler, value, 0).xyz;
#else
	return light.intensity;
#endif
}

light_sample sample_environment_light(light light, interaction reference, float2 value)
{
	float theta = value.y * ONE_PI;
	float phi = value.x * TWO_PI;

	float cos_theta = cos(theta);
	float sin_theta = sin(theta);

	light_sample sample;

	sample.pdf = sin_theta != 0 ? 1 / (TWO_PI * ONE_PI * sin_theta) : 0;
	sample.wi = normalize(mul(float4(spherical_direction(sin_theta, cos_theta, phi), 0), entities[light.entity].local_to_world).xyz);
	sample.interaction.position = reference.position + sample.wi * 2 * 1000;
	sample.interaction.wo = 0;
	sample.interaction.normal = 0;
#ifdef __ENABLE_ENVIRONMENT_IMAGE__
	sample.intensity = light.intensity * environment.SampleLevel(default_sampler, value, 0).xyz;
#else
	sample.intensity = light.intensity;
#endif
	
	return sample;
}

real pdf_environment_light(light light, interaction reference, float3 wi)
{
	float3 w = normalize(wi);

	float theta = spherical_theta(w);
	float phi = spherical_phi(w);

	float sin_theta = sin(theta);

	if (sin_theta == 0) return 0;

	return 1 / (TWO_PI * ONE_PI * sin_theta);
}

#endif