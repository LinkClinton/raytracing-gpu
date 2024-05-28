#include "../../materials/material_wrapper.hlsl"

struct frame_data
{
    float4x4 raster_to_camera;
    float4x4 camera_to_world;

    float3 camera_position;

    uint sample_index;
};

struct entity_data
{
    uint geometry_vtx_location;
    uint geometry_idx_location;

    uint material_index;
    uint light_index;

    uint material_type;
    uint light_type;
};

SHADER_RESOURCE_DEFINE(ConstantBuffer<frame_data>, global_frame_data, b0, space0);
SHADER_RESOURCE_DEFINE(StructuredBuffer<entity_data>, global_entities_data, t1, space0);
SHADER_RESOURCE_DEFINE(StructuredBuffer<texture_info>, global_materials, t2, space0);
SHADER_RESOURCE_DEFINE(StructuredBuffer<texture_info>, global_lights, t3, space0);

SHADER_RESOURCE_DEFINE(RaytracingAccelerationStructure, global_acceleration, t0, space1);
SHADER_RESOURCE_DEFINE(StructuredBuffer<float3>, global_geometry_positions, t1, space1);
SHADER_RESOURCE_DEFINE(StructuredBuffer<float3>, global_geometry_normals, t2, space1);
SHADER_RESOURCE_DEFINE(StructuredBuffer<float3>, global_geometry_uvs, t3, space1);
SHADER_RESOURCE_DEFINE(StructuredBuffer<uint3>,  global_geometry_indices, t4, space1);

SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, global_render_target_hdr, u0, space2);
SHADER_RESOURCE_DEFINE(RWTexture2D<float4>, global_render_target_sdr, u1, space2);

SHADER_RESOURCE_DEFINE(SamplerState, global_texture_sampler, s0, space10);

[shader("raygeneration")]
void ray_generation()
{
}