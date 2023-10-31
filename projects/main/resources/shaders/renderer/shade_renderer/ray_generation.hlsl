#include "../../types.hlsl"

struct frame_data
{
    float4x4 raster_to_camera;
    float4x4 camera_to_world;

    float3 camera_position;

    uint sample_index;
};

SHADER_RESOURCE_DEFINE(ConstantBuffer<frame_data>, global_frame_data, b0, space0);

SHADER_RESOURCE_DEFINE(RaytracingAccelerationStructure, global_acceleration, t0, space1);

SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, global_render_target_hdr, u0, space2);
SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, global_render_target_sdr, u1, space2);

[shader("raygeneration")]
void ray_generation()
{
    
}