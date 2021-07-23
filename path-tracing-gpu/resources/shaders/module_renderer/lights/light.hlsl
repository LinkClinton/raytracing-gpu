#ifndef __LIGHT_HLSL__
#define __LIGHT_HLSL__

#include "../module_types.hlsl"

struct light_search_result {
	surface_interaction interaction;

	uint light;
	real pdf;
};

light_search_result search_light(interaction interaction, float3 wi)
{
	light_search_result result;

	result.pdf = 0;

	ray_desc light_ray = interaction.spawn_ray(wi);

	tracing_payload light_payload;

	light_payload.missed = 0;

	TraceRay(acceleration, RAY_FLAG_FORCE_OPAQUE, 0xFF, 0,
		1, 0, light_ray, light_payload);

#ifdef __ENABLE_ENVIRONMENT_LIGHT__
	// if the ray does not intersect any entity and we enable and have environment light
	// we will return environment as search result
	if (light_payload.missed == 1 && config.environment != INDEX_NULL) {
		result.interaction.position = interaction.position + wi * 2 * 1000;
		result.light = config.environment;
		result.pdf = 1.0 / config.lights;

		return result;
	}
#endif

	if (light_payload.missed == 1 || entities[light_payload.entity].light == INDEX_NULL)
		return result;

	result.interaction = light_payload.interaction;
	result.light = entities[light_payload.entity].light;
	result.pdf = 1.0 / config.lights;

	return result;
}

// only used for coding, when we compile the ray generation shader
// we will define __HLSL_SHADER__ and write the real light structure
#ifndef __HLSL_SHADER__

struct light_sample {
	interaction interaction;

	float3 intensity;
	real pdf;

	float3 wi;
};

struct light {
	float3 intensity;
	uint entity;

	float3 direction;
	uint delta;

	uint type;
	float3 unused;
};

#ifdef __ENABLE_ENVIRONMENT_LIGHT__

float3 evaluate_environment_light(light light, interaction interaction, float3 wi)
{
	return float3(0, 0, 0);
}

#endif

float3 evaluate_light(light light, interaction interaction, float3 wi)
{
	return float3(0, 0, 0);
}

light_sample sample_light(light light, interaction reference, float2 value)
{
	light_sample sample;

	return sample;
}

real pdf_light(light light, interaction reference, float3 wi)
{
	return 0;
}

SHADER_STRUCTURED_BUFFER_DEFINE(light, lights, t1, space3);

#endif

#endif