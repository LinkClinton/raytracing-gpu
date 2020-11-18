#ifndef __DEPTH_TYPES_HLSL__
#define __DEPTH_TYPES_HLSL__

#include "../types.hlsl"

struct tracing_payload {
	uint missed;
	real depth;
};

struct scene_config {
	float4x4 raster_to_camera;
	float4x4 camera_to_world;

	float3 camera_position;
	
	uint sample_index;
};

SHADER_CONSTANT_BUFFER_DEFINE(scene_config, config, b0, space0);

SHADER_RESOURCE_DEFINE(RaytracingAccelerationStructure, acceleration, t0, space1);

SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, render_target_hdr, u0, space2);
SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, render_target_sdr, u1, space2);

#endif