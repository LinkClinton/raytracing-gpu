#include "../samplers/random_sampler.hlsl"

#include "materials/material.hlsl"
#include "lights/light.hlsl"
#include "module_types.hlsl"

void uniform_sample_one_light(real value, uint lights, out uint which, out real pdf)
{
	if (lights == 0) { which = 0; pdf = 0; return; }

	which = min(floor(value * config.lights), lights - 1);
	pdf = 1.0 / lights;
}

float3 uniform_sample_one_light(material material, tracing_payload payload, inout random_sampler sampler)
{
	float3 wo = world_to_local(payload.interaction.shading_space, payload.interaction.wo);
	float3 L = 0;

	{
		uint which = 0; float pdf = 0;

		uniform_sample_one_light(next_sample1d(sampler), config.lights, which, pdf);

		light_sample emitter_sample = sample_light(lights[which], payload.interaction.base_type(), next_sample2d(sampler));

		emitter_sample.pdf = emitter_sample.pdf * pdf;

		if (!is_black(emitter_sample.intensity) && emitter_sample.pdf > 0) {
			float3 wi = world_to_local(payload.interaction.shading_space, emitter_sample.wi);

			float3 function_value = evaluate_material(material, wo, wi);
			float function_pdf = pdf_material(material, wo, wi);

			function_value = function_value * abs(dot(emitter_sample.wi, payload.interaction.shading_space.z()));

			if (!is_black(function_value) && function_pdf > 0) {
				ray_desc shadow_ray = payload.interaction.spawn_ray_to(emitter_sample.interaction);
				tracing_payload shadow_payload;

				shadow_payload.missed = false;

				TraceRay(acceleration, SHADOW_FLAG, 0xFF, 0,
					1, 0, shadow_ray, shadow_payload);

				float weight = lights[which].delta == 1 ? 1 : power_heuristic(emitter_sample.pdf, function_pdf);

				if (shadow_payload.missed == true)
					L += function_value * emitter_sample.intensity * weight / emitter_sample.pdf;
			}
		}
	}

	{
		scattering_sample function_sample = sample_material(material, wo, next_sample2d(sampler));

		function_sample.wi = local_to_world(payload.interaction.shading_space, function_sample.wi);
		function_sample.value = function_sample.value * abs(dot(function_sample.wi, payload.interaction.shading_space.z()));

		if (!is_black(function_sample.value) && function_sample.pdf > 0) {

			light_search_result search_result = search_light(payload.interaction.base_type(), function_sample.wi);

			if (search_result.pdf > 0) {
				light light = lights[search_result.light];

				float3 light_value = evaluate_light(light, search_result.interaction.base_type(), -function_sample.wi);
				real light_pdf = pdf_light(light, payload.interaction.base_type(), function_sample.wi) * search_result.pdf;

				if (!is_black(light_value) && light_pdf > 0) {
					real weight = has(function_sample.type, scattering_specular) ? 1 :
						power_heuristic(function_sample.pdf, light_pdf);

					L += function_sample.value * light_value * weight / function_sample.pdf;
				}
			}
		}
	}

	return L;
}

float3 trace(ray_desc first, inout random_sampler sampler)
{
	path_tracing_info tracing_info;

	tracing_info.specular = 0;
	tracing_info.ray = first;
	tracing_info.value = 0;
	tracing_info.beta = 1;
	tracing_info.eta = 1;

	tracing_payload payload;

	payload.entity = INDEX_NUll;
	payload.missed = 0;
	
	for (int bounces = 0; bounces < config.max_depth; bounces++) {
		TraceRay(acceleration, RAY_FLAG_FORCE_OPAQUE, 0xFF, 0,
			1, 0, tracing_info.ray, payload);

		if (payload.missed != 0) break;

		entity_info entity = entities[payload.entity];
		
		if ((bounces == 0 || tracing_info.specular != 0) && entity.light != INDEX_NUll)
			tracing_info.value += tracing_info.beta * evaluate_light(lights[entity.light], 
				payload.interaction.base_type(), -tracing_info.ray.Direction);

		if (entity.material == INDEX_NUll) {
			tracing_info.ray = payload.interaction.spawn_ray(tracing_info.ray.Direction);

			bounces--;

			continue;
		}

		material material = unpacking_material(materials[entity.material], payload.interaction.uv);

		tracing_info.value += tracing_info.beta * uniform_sample_one_light(material, payload, sampler);

		float3 wo = world_to_local(payload.interaction.shading_space, payload.interaction.wo);

		scattering_sample function_sample = sample_material(material, wo, next_sample2d(sampler));

		if (is_black(function_sample.value) || function_sample.pdf == 0) 
			break;

		function_sample.wi = local_to_world(payload.interaction.shading_space, function_sample.wi);
		
		tracing_info.beta *= function_sample.value * abs(dot(function_sample.wi, payload.interaction.shading_space.z())) /
			function_sample.pdf;
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

	float3 L = trace(ray, sampler);

	real t = 1.0 / (config.sample_index + 1);

	float3 value = render_target_hdr[DispatchRaysIndex().xy].xyz;

	value = lerp(value, L, t);

	render_target_hdr[DispatchRaysIndex().xy] = float4(value, 1);
	render_target_sdr[DispatchRaysIndex().xy] = float4(value, 1);
}