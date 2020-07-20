#ifndef __EMITTER_HLSL__
#define __EMITTER_HLSL__

#include "emitter_environment.hlsl"
#include "emitter_include.hlsl"
#include "emitter_surface.hlsl"
#include "emitter_point.hlsl"

void uniform_sample_one_emitter(random_sampler sampler, uint emitters, out uint which, out uint pdf)
{
	if (emitters == 0) { which = 0; pdf = 0; return; }

	which = min(floor(next_sample1d(sampler) * emitters), emitters - 1);
	pdf = 1.0 / emitters;
}

float3 evaluate_emitter(emitter_gpu_buffer emitter, interaction interaction, float3 wi)
{
	if (emitter.type == emitter_point)
		return evaluate_point_emitter(emitter, interaction, wi);

	if (emitter.type == emitter_surface)
		return evaluate_surface_emitter(emitter, interaction, wi);

	if (emitter.type == emitter_environment)
		return evaluate_environment_emitter(emitter, interaction, wi);

	return float3(0, 0, 0);
}

emitter_sample sample_emitter(emitter_gpu_buffer emitter, interaction reference, float2 value)
{
	if (emitter.type == emitter_point)
		return sample_point_emitter(emitter, reference, value);

	if (emitter.type == emitter_surface)
		return sample_surface_emitter(emitter, reference, value);

	if (emitter.type == emitter_environment)
		return sample_environment_emitter(emitter, reference, value);
	
	emitter_sample sample; sample.pdf = 0;

	return sample;
}

float pdf_emitter(emitter_gpu_buffer emitter, interaction reference, float3 wi)
{
	if (emitter.type == emitter_point)
		return pdf_point_emitter(emitter, reference, wi);

	if (emitter.type == emitter_surface)
		return pdf_surface_emitter(emitter, reference, wi);

	if (emitter.type == emitter_environment)
		return pdf_environment_emitter(emitter, reference, wi);

	return 0;
}

#endif