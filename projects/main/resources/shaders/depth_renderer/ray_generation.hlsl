#include "../types.hlsl"

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
    float2 sample_location = ((DispatchRaysIndex().xy + 0.5f) / DispatchRaysDimensions().xy) * 2.0f - 1.0f;
    
    float3 ray_target_camera_space = mul(float4(sample_location, 0, 1), global_frame_data.raster_to_camera).xyz;
    float3 ray_origin_camera_space = float3(0, 0, 0);

    float3 ray_target_world_space = mul(float4(ray_target_camera_space, 1), global_frame_data.camera_to_world).xyz;
    float3 ray_origin_world_space = mul(float4(ray_origin_camera_space, 1), global_frame_data.camera_to_world).xyz;

    RayDesc ray;

    ray.Origin = ray_origin_world_space;
    ray.Direction = normalize(ray_target_world_space - ray_origin_world_space);
    ray.TMin = 0;
    ray.TMax = 1e20;

    RAY_QUERY(RAY_FLAG_FORCE_OPAQUE) query;

    query.TraceRayInline(
		global_acceleration,
		RAY_FLAG_FORCE_OPAQUE,
		0xFF, 
		ray);

    query.Proceed();

    float3 depth = global_render_target_hdr[DispatchRaysIndex().xy].rgb;
    float3 value = 0.f;

    if (query.CommittedStatus() == COMMITTED_TRIANGLE_HIT)
    {
        float3 hit_position = ray.Origin + ray.Direction * query.CommittedRayT();

        value = distance(global_frame_data.camera_position, hit_position) / 10.0f;
    }

    depth = lerp(depth, value, 1.0f / (global_frame_data.sample_index + 1));

    global_render_target_hdr[DispatchRaysIndex().xy] = float4(depth, 1);
    global_render_target_sdr[DispatchRaysIndex().xy] = float4(depth, 1);
}