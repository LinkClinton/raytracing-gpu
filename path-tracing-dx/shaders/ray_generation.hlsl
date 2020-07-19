#include "resource_scene.hlsl"

struct path_tracing_info {
	float3 value;
	float3 beta;

	float eta;

	bool specular;

	ray_desc ray;
};

float3 uniform_sample_one_emitter(random_sampler sampler, path_tracing_info tracing_info, ray_payload payload)
{
	float3 wo = world_to_local(payload.interaction.shading_space, payload.interaction.wo);
	float3 L = 0;
	
	scattering_type type = scattering_type(scattering_all ^ scattering_specular);

	{
		uint which = 0; float pdf = 0;
		
		uniform_sample_one_emitter(sampler, global_scene_info.emitters, which, pdf);

		emitter_sample emitter_sample = sample_emitter(global_emitters[which], payload.interaction.base_type(), next_sample2d(sampler));

		emitter_sample.pdf = emitter_sample.pdf * pdf;

		if (!is_black(emitter_sample.intensity) && emitter_sample.pdf > 0) {
			float3 wi = world_to_local(payload.interaction.shading_space, emitter_sample.wi);

			float3 function_value = evaluate_material(global_materials[payload.index], wo, wi, type);
			float function_pdf = pdf_material(global_materials[payload.index], wo, wi, type);

			function_value = function_value * abs(dot(emitter_sample.wi, payload.interaction.shading_space.z()));

			if (!is_black(function_value) && function_pdf > 0) {
				ray_desc shadow_ray = payload.interaction.spawn_ray_to(emitter_sample.interaction);
				ray_payload shadow_payload;

				shadow_payload.missed = false;

				TraceRay(global_acceleration, SHADOW_FLAG, 0xFF, 0,
					1, 0, shadow_ray, shadow_payload);

				float weight = 1;

				if (shadow_payload.missed == true)
					L += function_value * emitter_sample.intensity * weight / emitter_sample.pdf;
			}
		}
	}

	return L;
}

float3 trace(ray_desc first_ray, random_sampler sampler)
{
	path_tracing_info tracing_info;

	tracing_info.specular = false;
	tracing_info.ray = first_ray;
	tracing_info.value = 0;
	tracing_info.beta = 1;
	tracing_info.eta = 1;

	ray_payload payload;

	for (int bounces = 0; bounces < global_scene_info.max_depth; bounces++) {
		TraceRay(global_acceleration, RAY_FLAG_FORCE_OPAQUE, 0xFF, 0,
			1, 0, tracing_info.ray, payload);

		if (payload.missed == true) break;
		
		tracing_info.value += tracing_info.beta * uniform_sample_one_emitter(sampler, tracing_info, payload);

		float3 wo = world_to_local(payload.interaction.shading_space, payload.interaction.wo);

		scattering_sample function_sample = sample_material(global_materials[payload.index], wo, next_sample2d(sampler));

		if (is_black(function_sample.value) || function_sample.pdf == 0) break;

		function_sample.wi = local_to_world(payload.interaction.shading_space, function_sample.wi);

		tracing_info.beta *= function_sample.value * abs(dot(function_sample.wi, payload.interaction.normal)) / function_sample.pdf;

		tracing_info.specular = has(function_sample.type, scattering_specular);

		tracing_info.ray = payload.interaction.spawn_ray(function_sample.wi);
	}

	return tracing_info.value;
}

[shader("raygeneration")]
void ray_generation_shader() {
	random_sampler sampler = create_random_sampler(global_scene_info.sample_index);

	float2 ray_pixel_position = DispatchRaysIndex().xy + next_sample2d(sampler);
	
	float3 ray_target_camera_space = mul(float4(ray_pixel_position, 0, 1), global_scene_info.raster_to_camera).xyz;
	float3 ray_origin_camera_space = float3(0, 0, 0);
	
	float3 ray_target_world_space = mul(float4(ray_target_camera_space, 1), global_scene_info.camera_to_world).xyz;
	float3 ray_origin_world_space = mul(float4(ray_origin_camera_space, 1), global_scene_info.camera_to_world).xyz;

	RayDesc ray;

	ray.Origin = ray_origin_world_space;
	ray.Direction = normalize(ray_target_world_space.xyz - ray_origin_world_space.xyz);
	ray.TMin = 0;
	ray.TMax = 1e20;

	float3 L = trace(ray, sampler);
	
	float factor = global_scene_info.sample_index / (global_scene_info.sample_index + 1.0f);

	float3 old_value = global_render_target[DispatchRaysIndex().xy].xyz;
	float3 new_value = gamma_correct(L);

	global_render_target[DispatchRaysIndex().xy] = float4(lerp(new_value, old_value, factor), 1);
}