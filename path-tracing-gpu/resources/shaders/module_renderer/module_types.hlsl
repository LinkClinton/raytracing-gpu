#ifndef __MODULE_TYPES_HLSL__
#define __MODULE_TYPES_HLSL__

#include "../shared/interaction.hlsl"
#include "../types.hlsl"

struct tracing_payload {
	surface_interaction interaction;

	uint missed;
	uint entity;
};

struct scene_config {
	float4x4 raster_to_camera;
	float4x4 camera_to_world;

	float3 camera_position;

	uint sample_index;

	uint environment;
	uint max_depth;
	uint lights;
	uint unused0;
};

struct path_tracing_info {
	float3 value;
	uint specular;
	
	float3 beta;
	float eta;

	ray_desc ray;
};

struct mesh_info {
	uint triangle_count;

	uint vtx_location;
	uint idx_location;

	uint reverse;
	uint normals;
	uint uvs;
	
	float area;
};

struct entity_info {
	float4x4 local_to_world;
	float4x4 world_to_local;

	mesh_info mesh;
	
	uint material;
	uint light;
};

struct texture {
	float3 value;
	uint index;
};

SHADER_CONSTANT_BUFFER_DEFINE(scene_config, config, b0, space0);

SHADER_RESOURCE_DEFINE(RaytracingAccelerationStructure, acceleration, t0, space1);

SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, render_target_hdr, u0, space2);
SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, render_target_sdr, u1, space2);

SHADER_STRUCTURED_BUFFER_DEFINE(entity_info, entities, t0, space3);

SHADER_STRUCTURED_BUFFER_DEFINE(float3, positions, t0, space4);
SHADER_STRUCTURED_BUFFER_DEFINE(float3, normals, t1, space4);
SHADER_STRUCTURED_BUFFER_DEFINE(float3, uvs, t2, space4);

SHADER_STRUCTURED_BUFFER_DEFINE(uint3, indices, t3, space4);

SHADER_RESOURCE_DEFINE(Texture2D, environment, t0, space5);

SHADER_RESOURCE_DEFINE(Texture2D, textures[], t0, space6);

SHADER_RESOURCE_DEFINE(SamplerState, default_sampler, s0, space7);

#endif