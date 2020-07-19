#ifndef __EMITTER_INCLUDE_HLSL__
#define __EMITTER_INCLUDE_HLSL__

#include "../shared/interaction.hlsl"

enum emitter_type {
	emitter_unknown = 0,
	emitter_point = 1,
	emitter_surface = 2
};

struct emitter_gpu_buffer {
	emitter_type type;
	float3 intensity;

	float3 position;
	uint delta;

	uint index;
	float3 unused0;
};

struct emitter_sample {
	interaction interaction;

	float3 intensity;
	float3 wi;
	float pdf;
};

#endif