#ifndef __RESOURCE_SCENE_HLSL__
#define __RESOURCE_SCENE_HLSL__

#include "samplers/random_sampler.hlsl"
#include "materials/material.hlsl"
#include "emitters/emitter.hlsl"

struct scene_info {
	float4x4 raster_to_camera;
	float4x4 camera_to_world;

	float camera_focus;
	float camera_lens;

	uint sample_index;
	uint max_depth;
	uint emitters;

	float4x4 unused0;
	float4 unused1;
	float4 unused2;
	float3 unused3;
};

struct entity_gpu_buffer {
	float4x4 local_to_world;

	uint material;
	uint emitter;
	uint shape;
	uint unused;
};

struct shape_gpu_buffer {
	uint positions;
	uint normals;
	uint indices;
	uint uvs;
};

struct ray_payload {
	surface_interaction interaction;

	bool missed;
	
	uint index;
};

typedef BuiltInTriangleIntersectionAttributes HitAttributes;

SHADER_CONSTANT_BUFFER_DEFINE(scene_info, global_scene_info, b0, space0);
SHADER_RESOURCE_DEFINE(RaytracingAccelerationStructure, global_acceleration, t0, space1);
SHADER_STRUCTURED_BUFFER_DEFINE(entity_gpu_buffer, global_entities, t1, space1);
SHADER_STRUCTURED_BUFFER_DEFINE(material_gpu_buffer, global_materials, t2, space1);
SHADER_STRUCTURED_BUFFER_DEFINE(emitter_gpu_buffer, global_emitters, t3, space1);
SHADER_STRUCTURED_BUFFER_DEFINE(shape_gpu_buffer, global_shapes, t4, space1);

SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, global_render_target, u0, space2);

#endif