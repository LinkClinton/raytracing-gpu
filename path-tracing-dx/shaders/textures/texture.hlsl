#ifndef __TEXTURE_HLSL__
#define __TEXTURE_HLSL__

#include "../resource_scene.hlsl"
#include "texture_include.hlsl"

float3 sample_texture_if_no_null(texture_gpu_buffer texture, float2 value)
{
	return texture.scale * global_images[texture.index].SampleLevel(global_sampler, value, 0).xyz;
}

float3 sample_texture(texture_gpu_buffer texture, float2 value)
{
	if (texture.index == ENTITY_NUll) return texture.scale;

	return texture.scale * global_images[texture.index].SampleLevel(global_sampler, value, 0).xyz;
}

#endif