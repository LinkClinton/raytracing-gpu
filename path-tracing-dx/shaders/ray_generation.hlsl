#include "resource_scene.hlsl"

[shader("raygeneration")]
void ray_generation_shader() {
	global_render_target[DispatchRaysIndex().xy] = float4(1, 0, 0, 1);

	return;

	float2 ray_pixel_position = DispatchRaysIndex().xy + 0.5;
	float2 ndc_position = ray_pixel_position / (DispatchRaysDimensions().xy * 0.5) - 1.0;
	
	ndc_position.y *= -1.0;

	float4 ray_origin = mul(float4(ndc_position, 0, 1), global_scene_info.inv_view_projection);
	float4 ray_target = mul(float4(ndc_position, 1, 1), global_scene_info.inv_view_projection);

	ray_origin.xyz /= ray_origin.w;
	ray_target.xyz /= ray_target.w;

	RayDesc ray;
	ray.Origin = ray_origin.xyz;
	ray.Direction = normalize(ray_target.xyz - ray_origin.xyz);
	ray.TMin = 0;
	ray.TMax = length(ray_target.xyz - ray_origin.xyz);

	ray_payload payload;
	payload.spectrum = float3(0, 0, 0);

	TraceRay(global_acceleration, RAY_FLAG_FORCE_OPAQUE, 0xFFFFFFFF, 0, 1, 0, ray, payload);

	global_render_target[DispatchRaysIndex().xy].xyz = payload.spectrum;
}

[shader("closesthit")]
void closest_hit_shader(inout ray_payload payload, HitAttributes attribute) {
	payload.spectrum = float3(1, 0, 0);
}