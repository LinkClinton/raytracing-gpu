#ifndef __MATERIAL_INCLUDE_HLSL__
#define __MATERIAL_INCLUDE_HLSL__

#include "../scatterings/scattering_include.hlsl"

enum material_type {
	material_unknown = 0,
	material_diffuse = 1,
	material_mirror = 2,
	material_substrate = 3
};

struct material_gpu_buffer {
	material_type type;
	uint reflectance;
	uint specular;
	uint diffuse;
	
	uint roughness_u;
	uint roughness_v;
	uint remapped;
	
	uint unused;
};

struct material_shader_buffer {
	material_type type;

	float3 reflectance;
	float3 specular;
	float3 diffuse;
	float roughness_u;
	float roughness_v;
};

#endif