#ifndef __RESOURCE_SCENE_HLSL__
#define __RESOURCE_SCENE_HLSL__

#include "samplers/random_sampler.hlsl"
#include "materials/material_include.hlsl"
#include "emitters/emitter_include.hlsl"
#include "textures/texture_include.hlsl"

struct scene_info {
	float4x4 raster_to_camera;
	float4x4 camera_to_world;

	float camera_focus;
	float camera_lens;

	uint sample_index;
	uint max_depth;
	uint emitters;
	uint environment;
	uint texture;
	
	float4x4 unused0;
	float4 unused1;
	float4 unused2;
	float1 unused3;
};

struct entity_gpu_buffer {
	float4x4 local_to_world;
	float4x4 world_to_local;

	uint material;
	uint emitter;
	uint shape;
	float area;
};

struct shape_gpu_buffer {
	uint positions;
	uint normals;
	uint indices;
	uint uvs;

	uint reverse;

	uint3 unused;
};

struct ray_payload {
	surface_interaction interaction;

	bool missed;

	uint index;
};

typedef BuiltInTriangleIntersectionAttributes HitAttributes;

SHADER_CONSTANT_BUFFER_DEFINE(scene_info, global_scene_info, b0, space0);
SHADER_RESOURCE_DEFINE(RaytracingAccelerationStructure, global_acceleration, t0, space1);
SHADER_STRUCTURED_BUFFER_DEFINE(material_gpu_buffer, global_materials, t1, space1);
SHADER_STRUCTURED_BUFFER_DEFINE(texture_gpu_buffer, global_textures, t2, space1);
SHADER_STRUCTURED_BUFFER_DEFINE(emitter_gpu_buffer, global_emitters, t3, space1);
SHADER_STRUCTURED_BUFFER_DEFINE(entity_gpu_buffer, global_entities, t4, space1);
SHADER_STRUCTURED_BUFFER_DEFINE(shape_gpu_buffer, global_shapes, t5, space1);

SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, global_render_target, u0, space2);

SHADER_STRUCTURED_BUFFER_DEFINE(float3, global_positions[], t0, space3);
SHADER_STRUCTURED_BUFFER_DEFINE(float3, global_normals[], t0, space4);
SHADER_STRUCTURED_BUFFER_DEFINE(float3, global_uvs[], t0, space6);

SHADER_STRUCTURED_BUFFER_DEFINE(uint3, global_indices[], t0, space5);

SHADER_RESOURCE_DEFINE(Texture2D, global_images[], t0, space7);

SHADER_RESOURCE_DEFINE(SamplerState, global_sampler, s0, space20);

#endif