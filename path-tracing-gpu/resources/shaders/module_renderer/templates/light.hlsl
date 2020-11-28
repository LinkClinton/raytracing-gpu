#ifndef __HLSL_SHADER__
#include "../../shared/interaction.hlsl"
#include "../../types.hlsl"
#endif

struct light_sample {
	interaction interaction;

	float3 intensity;
	real pdf;

	float3 wi;
};

struct light {
	// [light]
	// [light]

#ifndef __HLSL_SHADER__
	uint type;
#endif
};

SHADER_STRUCTURED_BUFFER_DEFINE(light, lights, t1, space3);

// [include_light_submodule]
// [include_light_submodule]

float3 evaluate_light(light light, interaction interaction, float3 wi)
{
	switch (light.type) {
	// [evaluate_light]
	// [evaluate_light]
	default: return 0;
	}
}

light_sample sample_light(light light, interaction reference, float2 value)
{
	light_sample sample; sample.pdf = 0;

	switch (light.type) {
	// [sample_light]
	// [sample_light]
	default: return sample;
	}
}

real pdf_light(light light, interaction reference, float3 wi)
{
	switch (light.type) {
	// [pdf_light]
	// [pdf_light]
	default: return 0;
	}
}