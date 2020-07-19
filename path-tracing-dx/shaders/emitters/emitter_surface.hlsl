#ifndef __EMITTER_SURFACE_HLSL__
#define __EMITTER_SURFACE_HLSL__

#include "emitter_include.hlsl"

#include "../shapes/shape_sampling.hlsl"

float3 evaluate_surface_emitter(emitter_gpu_buffer emitter, interaction interaction, float3 wi)
{
	return (dot(interaction.normal, wi) > 0) ? emitter.intensity : 0;
}

emitter_sample sample_surface_emitter(emitter_gpu_buffer emitter, interaction reference, float2 value)
{
	emitter_sample sample;
	
	shape_sample shape_sample = sample_triangles_with_reference(emitter.index, reference, value);

	if (shape_sample.pdf == 0) { sample.pdf = 0; return sample; }

	sample.wi = normalize(shape_sample.interaction.position - reference.position);
	sample.interaction = shape_sample.interaction;
	sample.intensity = evaluate_surface_emitter(emitter, shape_sample.interaction, -sample.wi);
	sample.pdf = shape_sample.pdf;

	return sample;
}

float pdf_surface_emitter(emitter_gpu_buffer emitter, interaction reference, float3 wi)
{
	return pdf_triangles_with_reference(emitter.index, reference, wi);
}

#endif