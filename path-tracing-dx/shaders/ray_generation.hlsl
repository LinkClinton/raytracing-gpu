#include "resource_scene.hlsl"

[shader("raygeneration")]
void ray_generation_shader() {
	float2 ray_pixel_position = DispatchRaysIndex().xy + 0.5;
	
	float3 ray_target_camera_space = mul(float4(ray_pixel_position, 0, 1), global_scene_info.raster_to_camera).xyz;
	float3 ray_origin_camera_space = float3(0, 0, 0);
	
	float3 ray_target_world_space = mul(float4(ray_target_camera_space, 1), global_scene_info.camera_to_world).xyz;
	float3 ray_origin_world_space = mul(float4(ray_origin_camera_space, 1), global_scene_info.camera_to_world).xyz;

	RayDesc ray;
	ray.Origin = ray_origin_world_space.xyz;
	ray.Direction = normalize(ray_target_world_space.xyz - ray_origin_world_space.xyz);
	ray.TMin = 0;
	ray.TMax = 1e20;

	ray_payload payload;
	payload.spectrum = float3(0, 0, 0);

	TraceRay(global_acceleration, RAY_FLAG_FORCE_OPAQUE, 0xFFFFFFFF, 0, 1, 0, ray, payload);

	global_render_target[DispatchRaysIndex().xy] = float4(payload.spectrum, 1);
}

[shader("closesthit")]
void closest_hit_shader(inout ray_payload payload, HitAttributes attribute) {
	payload.spectrum = float3(1, 0, 0);
}