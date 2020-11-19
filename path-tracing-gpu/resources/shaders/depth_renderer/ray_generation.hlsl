#include "depth_types.hlsl"

#include "../samplers/random_sampler.hlsl"

float3 trace(RayDesc ray)
{
	tracing_payload payload;

	payload.missed = 0;
	payload.depth = 0;
	
	TraceRay(acceleration, RAY_FLAG_FORCE_OPAQUE, 0xFF,
		0, 1, 0, ray, payload);

	return float3(payload.depth, payload.depth, payload.depth);
}

[shader("raygeneration")]
void ray_generation()
{
	random_sampler sampler = create_random_sampler(config.sample_index);
	
	float2 ray_pixel_location = DispatchRaysIndex().xy + next_sample2d(sampler).xy;

	float3 ray_target_camera_space = mul(float4(ray_pixel_location, 0, 1), config.raster_to_camera).xyz;
	float3 ray_origin_camera_space = float3(0, 0, 0);
	float3 ray_direction_camera_space = normalize(ray_target_camera_space - ray_origin_camera_space);

	float3 ray_target_world_space = mul(float4(ray_target_camera_space, 1), config.camera_to_world).xyz;
	float3 ray_origin_world_space = mul(float4(ray_origin_camera_space, 1), config.camera_to_world).xyz;

	RayDesc ray;

	ray.Origin = ray_origin_world_space;
	ray.Direction = normalize(ray_target_world_space.xyz - ray_origin_world_space.xyz);
	ray.TMin = 0;
	ray.TMax = 1e20;

	float3 L = trace(ray);

	real t = 1.0 / (config.sample_index + 1);
	
	float3 value = render_target_hdr[DispatchRaysIndex().xy].xyz;

	value = lerp(value, L, t);

	render_target_hdr[DispatchRaysIndex().xy] = float4(value, 1);
	render_target_sdr[DispatchRaysIndex().xy] = float4(value, 1);
}