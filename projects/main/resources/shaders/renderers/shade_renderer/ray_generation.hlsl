#include "../../types.hlsl"

struct frame_data
{
    float4x4 raster_to_camera;
    float4x4 camera_to_world;

    float3 camera_position;

    uint sample_index;
};

struct entity_info
{
    uint geometry_index;
};

SHADER_RESOURCE_DEFINE(ConstantBuffer<frame_data>, global_frame_data, b0, space0);

SHADER_RESOURCE_DEFINE(RaytracingAccelerationStructure, global_acceleration, t0, space1);

SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, global_render_target_hdr, u0, space2);
SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, global_render_target_sdr, u1, space2);

SHADER_RESOURCE_DEFINE(StructuredBuffer<float3>, global_geometry_positions[], t0, space3);
SHADER_RESOURCE_DEFINE(StructuredBuffer<float3>, global_geometry_normals[], t0, space4);
SHADER_RESOURCE_DEFINE(StructuredBuffer<float3>, global_geometry_uvs[], t0, space5);
SHADER_RESOURCE_DEFINE(StructuredBuffer<uint3>,  global_geometry_indices[], t0, space6);

[shader("raygeneration")]
void ray_generation()
{
    
}