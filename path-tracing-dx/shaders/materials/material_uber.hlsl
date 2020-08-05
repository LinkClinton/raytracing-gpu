#ifndef __MATERIAL_UBER_HLSL__
#define __MATERIAL_UBER_HLSL__

#include "../scatterings/scattering_lambertian.hlsl"
#include "../scatterings/scattering_microfacet.hlsl"
#include "../emitters/emitter.hlsl"
#include "material_include.hlsl"

struct uber_material {
	float3 specular;
	float3 diffuse;
	float3 eta;

	float roughness_u;
	float roughness_v;
};

uber_material convert_uber_material(material_gpu_buffer buffer, float2 value)
{
	uber_material material;

	material.specular = sample_texture(global_textures[buffer.specular], value);
	material.diffuse = sample_texture(global_textures[buffer.diffuse], value);
	material.eta = sample_texture(global_textures[buffer.eta], value);
	material.roughness_u = sample_texture(global_textures[buffer.roughness_u], value).x;
	material.roughness_v = sample_texture(global_textures[buffer.roughness_v], value).x;

	if (buffer.remapped != 0) {
		material.roughness_u = roughness_to_alpha(material.roughness_u);
		material.roughness_v = roughness_to_alpha(material.roughness_v);
	}

	return material;
}

float3 evaluate_uber_material(uber_material material, float3 wo, float3 wi)
{
	if (wo.z == 0 || same_hemisphere(wo, wi) == false) return 0;

	float3 L = 0;

	L += evaluate_lambertian_reflection(material.diffuse, wo, wi);
	L += evaluate_dielectric_microfacet_reflection(material.specular, material.eta.x, material.roughness_u, material.roughness_v,
		wo, wi);

	return L;
}

float3 evaluate_uber_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (wo.z == 0 || same_hemisphere(wo, wi) == false) return 0;

	float3 L = 0;

	if (has(include, scattering_type(scattering_reflection | scattering_diffuse)))
		L += evaluate_lambertian_reflection(material.diffuse, wo, wi);

	if (has(include, scattering_type(scattering_reflection | scattering_glossy)))
		L += evaluate_dielectric_microfacet_reflection(material.specular, material.eta.x, material.roughness_u, material.roughness_v,
			wo, wi);

	return L;
}

float pdf_uber_material(uber_material material, float3 wo, float3 wi)
{
	float pdf = 0;

	pdf += pdf_lambertian_reflection(wo, wi);
	pdf += pdf_microfacet_reflection(material.roughness_u, material.roughness_v, wo, wi);

	return pdf / 2.0;
}

float pdf_uber_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	float pdf = 0;
	uint count = 0;

	if (has(include, scattering_type(scattering_reflection | scattering_diffuse))) {
		pdf += pdf_lambertian_reflection(wo, wi);
		count += 1;
	}

	if (has(include, scattering_type(scattering_reflection | scattering_glossy))) {
		pdf += pdf_microfacet_reflection(material.roughness_u, material.roughness_v, wo, wi);
		count += 1;
	}

	return count > 1 ? pdf / count : pdf;
}

scattering_sample sample_uber_material(uber_material material, float3 wo, float2 value)
{
	scattering_sample sample;

	// sample first bsdf
	if (value.x < 0.5) {
		value.x = min(value.x * 2 - 0, 1 - EPSILON);

		sample.type = scattering_type(scattering_reflection | scattering_diffuse);
		sample.wi = sample_lambertian_reflection(material.diffuse, wo, value).wi;
	}
	else {
		value.x = min(value.x * 2 - 1, 1 - EPSILON);

		sample.type = scattering_type(scattering_reflection | scattering_glossy);;
		sample.wi = sample_dielectric_microfacet_reflection(material.specular, material.eta.x,
			material.roughness_u, material.roughness_v, wo, value).wi;
	}

	sample.value = evaluate_uber_material(material, wo, sample.wi);
	sample.pdf = pdf_uber_material(material, wo, sample.wi);

	return sample;
}

scattering_sample sample_uber_material(material_shader_buffer material, float3 wo, float2 value, scattering_type include = scattering_all)
{
	scattering_sample sample;

	// todo : remove include
	// sample first bsdf
	if (value.x < 0.5) {
		value.x = min(value.x * 2 - 0, 1 - EPSILON);

		sample.type = scattering_type(scattering_reflection | scattering_diffuse);
		sample.wi = sample_lambertian_reflection(material.diffuse, wo, value).wi;
	}
	else {
		value.x = min(value.x * 2 - 1, 1 - EPSILON);

		sample.type = scattering_type(scattering_reflection | scattering_glossy);
		sample.wi = sample_dielectric_microfacet_reflection(material.specular, material.eta.x,
			material.roughness_u, material.roughness_v, wo, value).wi;
	}

	sample.value = evaluate_uber_material(material, wo, sample.wi, include);
	sample.pdf = pdf_uber_material(material, wo, sample.wi, include);

	return sample;
}

float3 uniform_sample_one_emitter_with_uber_material(uber_material material, inout ray_payload payload)
{
	float3 wo = world_to_local(payload.interaction.shading_space, payload.interaction.wo);
	float3 L = 0;

	{
		uint which = 0; float pdf = 0;

		uniform_sample_one_emitter(payload.sampler, global_scene_info.emitters, which, pdf);

		emitter_sample emitter_sample = sample_emitter(global_emitters[which], payload.interaction.base_type(), next_sample2d(payload.sampler));

		emitter_sample.pdf = emitter_sample.pdf * pdf;

		if (!is_black(emitter_sample.intensity) && emitter_sample.pdf > 0) {
			float3 wi = world_to_local(payload.interaction.shading_space, emitter_sample.wi);

			float3 function_value = evaluate_uber_material(material, wo, wi);
			float function_pdf = pdf_uber_material(material, wo, wi);

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
		scattering_sample function_sample = sample_uber_material(material, wo, next_sample2d(payload.sampler));

		function_sample.wi = local_to_world(payload.interaction.shading_space, function_sample.wi);
		function_sample.value = function_sample.value * abs(dot(function_sample.wi, payload.interaction.shading_space.z()));

		if (!is_black(function_sample.value) && function_sample.pdf > 0) {

			emitter_search_result search_result = search_emitter(payload.interaction.base_type(), function_sample.wi);

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

bool sample_uber_material_and_lighting(inout ray_payload payload)
{
	uber_material material = convert_uber_material(global_materials[global_entities[payload.index].material], payload.interaction.uv);

	payload.value = uniform_sample_one_emitter_with_uber_material(material, payload);

	float3 wo = world_to_local(payload.interaction.shading_space, payload.interaction.wo);

	scattering_sample function_sample = sample_uber_material(material, wo, next_sample2d(payload.sampler));

	if (is_black(function_sample.value) || function_sample.pdf == 0) {
		payload.function_value = 0;
		payload.function_pdf = 0;

		return false;
	}

	payload.function_wi = local_to_world(payload.interaction.shading_space, function_sample.wi);
	payload.function_value = function_sample.value;
	payload.function_pdf = function_sample.pdf;
	
	payload.specular = has(scattering_type(function_sample.type), scattering_specular);

	return true;
}

#endif