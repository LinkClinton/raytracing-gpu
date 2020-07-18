#ifndef __RESOURCE_SCENE_HLSL__
#define __RESOURCE_SCENE_HLSL__

#include "samplers/random_sampler.hlsl"
#include "materials/material.hlsl"
#include "emitters/emitter.hlsl"

typedef uint64_t uint64;

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

struct ray_payload {
	surface_interaction interaction;

	bool missed;
	
	uint index;
};

typedef BuiltInTriangleIntersectionAttributes HitAttributes;

ConstantBuffer<scene_info> global_scene_info : register(b0, space0);
RaytracingAccelerationStructure global_acceleration : register(t0, space1);
StructuredBuffer<material_gpu_buffer> global_materials : register(t1, space1);
StructuredBuffer<emitter_gpu_buffer> global_emitters : register(t2, space1);

RWTexture2D<float4> global_render_target : register(u0, space2);

#endif