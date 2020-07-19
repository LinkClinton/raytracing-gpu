#ifndef __SHAPE_INCLUDE_HLSL__
#define __SHAPE_INCLUDE_HLSL__

#include "../shared/interaction.hlsl"

struct shape_sample {
	interaction interaction;
	float pdf;
};

#endif