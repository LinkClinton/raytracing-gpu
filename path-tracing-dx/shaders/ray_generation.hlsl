#include "materials/material.hlsl"
#include "emitters/emitter.hlsl"
#include "resource_scene.hlsl"

struct path_tracing_info {
	float3 value;
	float3 beta;

	float eta;

	bool specular;

	ray_desc ray;
};

struct emitter_search_result {
	surface_interaction interaction;

	uint emitter;
	
	float pdf;
};

emitter_search_result search_emitter(random_sampler sampler, interaction interaction, float3 wi)
{
	emitter_search_result result;

	result.pdf = 0;
	
	ray_desc emitter_ray = interaction.spawn_ray(wi);
	ray_payload emitter_payload;

	emitter_payload.missed = false;

	TraceRay(global_acceleration, RAY_FLAG_FORCE_OPAQUE, 0xFF, 0,
		1, 0, emitter_ray, emitter_payload);

	if (emitter_payload.missed == true && global_scene_info.environment != ENTITY_NUll) {
		result.interaction.position = interaction.position + wi * 2 * 1000;
		result.emitter = global_scene_info.environment;
		result.pdf = result.pdf = 1.0 / global_scene_info.emitters;

		return result;
	}
	
	if (emitter_payload.missed == true || global_entities[emitter_payload.index].emitter == ENTITY_NUll)
		return result;
	
	result.interaction = emitter_payload.interaction;
	result.emitter = global_entities[emitter_payload.index].emitter;
	result.pdf = 1.0 / global_scene_info.emitters;

	return result;
}

void uniform_sample_one_emitter(random_sampler sampler, uint emitters, out uint which, out float pdf)
{
	if (emitters == 0) { which = 0; pdf = 0; return; }

	which = min(floor(next_sample1d(sampler) * emitters), emitters - 1);
	pdf = 1.0 / emitters;
}

float3 uniform_sample_one_emitter(random_sampler sampler, material_shader_buffer material, path_tracing_info tracing_info, ray_payload payload)
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
			
			float3 function_value = evaluate_material(material, wo, wi, type);
			float function_pdf = pdf_material(material, wo, wi, type);

			function_value = function_value * abs(dot(emitter_sample.wi, payload.interaction.shading_space.z()));
			
			if (!is_black(function_value) && function_pdf > 0) {
				ray_desc shadow_ray = payload.interaction.spawn_ray_to(emitter_sample.interaction);
				ray_payload shadow_payload;
				
				shadow_payload.missed = false;

				TraceRay(global_acceleration, SHADOW_FLAG, 0xFF, 0,
					1, 0, shadow_ray, shadow_payload);

				float weight = global_emitters[which].delta == 1 ? 1 : power_heuristic(emitter_sample.pdf, function_pdf);

				if (shadow_payload.missed == true)
					L += function_value * emitter_sample.intensity * weight / emitter_sample.pdf;
			}
		}
	}
	
	{
		scattering_sample function_sample = sample_material(material, wo, next_sample2d(sampler), type);
		
		function_sample.wi = local_to_world(payload.interaction.shading_space, function_sample.wi);
		function_sample.value = function_sample.value * abs(dot(function_sample.wi, payload.interaction.shading_space.z()));
		
		if (!is_black(function_sample.value) && function_sample.pdf > 0) {
			
			emitter_search_result search_result = search_emitter(sampler, payload.interaction.base_type(), function_sample.wi);

			if (search_result.pdf > 0) {
				emitter_gpu_buffer emitter = global_emitters[search_result.emitter];
				
				float3 emitter_value = evaluate_emitter(emitter, search_result.interaction.base_type(), -function_sample.wi);
				float emitter_pdf = pdf_emitter(emitter, payload.interaction.base_type(), function_sample.wi) * search_result.pdf;

				if (!is_black(emitter_value) && emitter_pdf > 0) {
					float weight = has(function_sample.type, scattering_specular) ? 1 :
						power_heuristic(function_sample.pdf, emitter_pdf);
					
					L += function_sample.value * emitter_value * weight / function_sample.pdf;
				}
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

		material_shader_buffer material = convert_gpu_buffer_to_shader_buffer(global_materials[global_entities[payload.index].material], payload.interaction.uv);
		
		tracing_info.value += tracing_info.beta * uniform_sample_one_emitter(sampler, material, tracing_info, payload);

		float3 wo = world_to_local(payload.interaction.shading_space, payload.interaction.wo);

		scattering_sample function_sample = sample_material(material, wo, next_sample2d(sampler));

		if (is_black(function_sample.value) || function_sample.pdf == 0) break;

		function_sample.wi = local_to_world(payload.interaction.shading_space, function_sample.wi);

		tracing_info.beta *= function_sample.value * abs(dot(function_sample.wi, payload.interaction.shading_space.z())) / function_sample.pdf;

		tracing_info.specular = has(function_sample.type, scattering_specular);

		tracing_info.ray = payload.interaction.spawn_ray(function_sample.wi);

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
	
	float factor = global_scene_info.sample_index / (global_scene_info.sample_index + 1.0f);
	
	float3 old_value = global_scene_info.sample_index == 0 ? 0 : global_render_target[DispatchRaysIndex().xy].xyz;
	float3 new_value = gamma_correct(L);

	global_render_target[DispatchRaysIndex().xy] = float4(lerp(new_value, old_value, factor), 1);
}