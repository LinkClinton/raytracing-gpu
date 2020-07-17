#ifndef __MATERIAL_INCLUDE_HLSL__
#define __MATERIAL_INCLUDE_HLSL__

#include "../scatterings/scattering_include.hlsl"

enum material_type {
	material_unknown = 0,
	material_diffuse = 1
};

struct material_gpu_buffer {
	material_type type;
	float3 diffuse;
};

#endif