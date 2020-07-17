#ifndef __EMITTER_POINT_HLSL__
#define __EMITTER_POINT_HLSL__

#include "emitter_include.hlsl"

float3 evaluate_point_emitter(emitter_gpu_buffer emitter, interaction interaction, float3 wi)
{
	return float3(0, 0, 0);
}

emitter_sample sample_point_emitter(emitter_gpu_buffer emitter, interaction reference, float2 value) 
{
	emitter_sample sample;

	sample.interaction.position = emitter.position;
	sample.interaction.normal = float3(0, 0, 0);
	sample.interaction.wo = float3(0, 0, 0);

	sample.intensity = emitter.intensity / distance_squared(emitter.position, reference.position);
	sample.wi = normalize(emitter.position - reference.position);
	sample.pdf = 1;

	return sample;
}

float pdf_point_emitter(emitter_gpu_buffer emitter, interaction reference, float3 wi)
{
	return 0;
}

#endif