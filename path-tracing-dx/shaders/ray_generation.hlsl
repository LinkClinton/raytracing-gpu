#include "materials/material.hlsl"
#include "emitters/emitter.hlsl"
#include "resource_scene.hlsl"

float3 trace(ray_desc first_ray, random_sampler sampler)
{
	path_tracing_info tracing_info;

	tracing_info.specular = false;
	tracing_info.ray = first_ray;
	tracing_info.value = 0;
	tracing_info.beta = 1;
	tracing_info.eta = 1;

	ray_payload payload;

	payload.sampler = sampler;
	payload.shadow = false;
	
	for (int bounces = 0; bounces < global_scene_info.max_depth; bounces++) {
		TraceRay(global_acceleration, RAY_FLAG_FORCE_OPAQUE, 0xFF, 0,
			1, 0, tracing_info.ray, payload);

		if (payload.missed == true) {
			if ((bounces != 0 && !tracing_info.specular) || global_scene_info.environment == ENTITY_NUll) break;
			
			interaction interaction;
			
			tracing_info.value += tracing_info.beta * evaluate_environment_emitter(global_emitters[global_scene_info.environment],
				interaction, -tracing_info.ray.Direction);
			
			break;
		}
		
		if ((bounces == 0 || tracing_info.specular) && global_entities[payload.index].emitter != ENTITY_NUll)
			tracing_info.value += tracing_info.beta * evaluate_emitter(global_emitters[global_entities[payload.index].emitter],
				payload.interaction.base_type(), -tracing_info.ray.Direction);

		if (global_entities[payload.index].material == ENTITY_NUll) {
			tracing_info.ray = payload.interaction.spawn_ray(tracing_info.ray.Direction);

			bounces--;

			continue;
		}

		tracing_info.value += tracing_info.beta * payload.value;

		if (is_black(payload.function_value) || payload.function_pdf == 0) break;
		
		tracing_info.beta *= payload.function_value * abs(dot(payload.function_wi, payload.interaction.shading_space.z())) / 
			payload.function_pdf;

		tracing_info.specular = payload.specular;
		
		tracing_info.ray = payload.interaction.spawn_ray(payload.function_wi);
		
		float component = max_component(tracing_info.beta);

		if (component < 1 && bounces > 3) {
			float q = max(0.05, 1 - component);

			if (next_sample1d(sampler) < q) break;

			tracing_info.beta /= (1 - q);
		}
	}

	return tracing_info.value;
}

[shader("raygeneration")]
void ray_generation_shader() {
	random_sampler sampler = create_random_sampler(global_scene_info.sample_index);

	float2 ray_pixel_position = DispatchRaysIndex().xy + next_sample2d(sampler);
	
	float3 ray_target_camera_space = mul(float4(ray_pixel_position, 0, 1), global_scene_info.raster_to_camera).xyz;
	float3 ray_origin_camera_space = float3(0, 0, 0);
	float3 ray_direction_camera_space = normalize(ray_target_camera_space - ray_origin_camera_space);
	
	if (global_scene_info.camera_lens != 0) {
		float2 lens = concentric_sample_disk(next_sample2d(sampler)) * global_scene_info.camera_lens;

		ray_target_camera_space = ray_origin_camera_space + ray_direction_camera_space * (global_scene_info.camera_focus / ray_direction_camera_space.z);
		ray_origin_camera_space = float3(lens.x, lens.y, 0);
	}
	
	float3 ray_target_world_space = mul(float4(ray_target_camera_space, 1), global_scene_info.camera_to_world).xyz;
	float3 ray_origin_world_space = mul(float4(ray_origin_camera_space, 1), global_scene_info.camera_to_world).xyz;
	
	RayDesc ray;

	ray.Origin = ray_origin_world_space;
	ray.Direction = normalize(ray_target_world_space.xyz - ray_origin_world_space.xyz);
	ray.TMin = 0;
	ray.TMax = 1e20;

	float3 L = trace(ray, sampler);

	uint count = global_scene_info.sample_index + 1;

	if (global_scene_info.sample_index == 0) global_render_target_hdr[DispatchRaysIndex().xy] = float4(0, 0, 0, 0);
	
	global_render_target_hdr[DispatchRaysIndex().xy] += float4(L, 0);
	global_render_target_sdr[DispatchRaysIndex().xy] = 
		float4(gamma_correct(global_render_target_hdr[DispatchRaysIndex().xy].xyz * global_scene_info.scale / count), 1);
}