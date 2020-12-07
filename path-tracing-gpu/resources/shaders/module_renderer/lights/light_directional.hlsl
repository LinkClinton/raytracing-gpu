#ifndef __LIGHT_DIRECTIONAL_HLSL__
#define __LIGHT_DIRECTIONAL_HLSL__

#include "light.hlsl"

float3 evaluate_directional_light(light light, interaction interaction, float3 wi)
{
	return 0;
}

light_sample sample_directional_light(light light, interaction reference, float2 value)
{
	light_sample sample;

	sample.interaction.position = reference.position + light.direction * 2 * 1000.f;
	sample.interaction.normal = 0;
	sample.interaction.wo = 0;
	sample.intensity = light.intensity;
	sample.wi = light.direction;
	sample.pdf = 1;

	return sample;
}

real pdf_directional_light(light light, interaction reference, float3 wi)
{
	return 0;
}

#endif