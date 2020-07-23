#ifndef __EMITTER_ENVIRONMENT_HLSL__
#define __EMITTER_ENVIRONMENT_HLSL__

#include "../textures/texture.hlsl"
#include "../resource_scene.hlsl"
#include "emitter_include.hlsl"

float3 evaluate_environment_emitter(emitter_gpu_buffer emitter, interaction interaction, float3 wi)
{
	float3 w = normalize(-mul(float4(wi, 0), global_entities[emitter.index].world_to_local).xyz);

	float2 value = float2(spherical_phi(w) * ONE_OVER_TWO_PI, spherical_theta(w) * ONE_OVER_PI);
	
	return (global_scene_info.texture != ENTITY_NUll ? sample_texture_if_no_null(global_textures[global_scene_info.texture], value) : 1) * emitter.intensity;
}

emitter_sample sample_environment_emitter(emitter_gpu_buffer emitter, interaction reference, float2 value)
{
	float theta = value.y * ONE_PI;
	float phi = value.x * TWO_PI;

	float cos_theta = cos(theta);
	float sin_theta = sin(theta);

	emitter_sample sample;

	sample.pdf = sin_theta != 0 ? 1 / (TWO_PI * ONE_PI * sin_theta) : 0;
	sample.wi = normalize(mul(float4(spherical_direction(sin_theta, cos_theta, phi), 0), global_entities[emitter.index].local_to_world).xyz);
	sample.interaction.position = reference.position + sample.wi * 2 * 1000;
	sample.interaction.wo = 0;
	sample.interaction.normal = 0;
	sample.intensity = (global_scene_info.texture != ENTITY_NUll ? sample_texture_if_no_null(global_textures[global_scene_info.texture], value) : 1) * emitter.intensity;

	return sample;
}

float pdf_environment_emitter(emitter_gpu_buffer emitter, interaction reference, float3 wi)
{
	float3 w = normalize(wi);

	float theta = spherical_theta(w);
	float phi = spherical_phi(w);

	float sin_theta = sin(theta);

	if (sin_theta == 0) return 0;

	return 1 / (TWO_PI * ONE_PI * sin_theta);
}

#endif