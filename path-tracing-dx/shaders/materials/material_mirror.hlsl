#ifndef __MATERIAL_MIRROR_HLSL__
#define __MATERIAL_MIRROR_HLSL__

#include "../scatterings/scattering_specular.hlsl"
#include "../emitters/emitter.hlsl"
#include "material_include.hlsl"

struct mirror_material {
	float3 reflectance;
};

mirror_material convert_mirror_material(material_gpu_buffer buffer, float2 value)
{
	mirror_material material;

	material.reflectance = sample_texture(global_textures[buffer.reflectance], value);

	return material;
}

float3 evaluate_mirror_material(mirror_material material, float3 wo, float3 wi)
{
	if (wo.z == 0 || same_hemisphere(wo, wi) == false) return 0;

	return evaluate_specular_reflection(material.reflectance, wo, wi);
}

float3 evaluate_mirror_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (wo.z == 0 || same_hemisphere(wo, wi) == false) return 0;

	if (has(include, scattering_type(scattering_reflection | scattering_specular)))
		return evaluate_specular_reflection(material.reflectance, wo, wi);

	return 0;
}

float pdf_mirror_material(mirror_material material, float3 wo, float3 wi)
{
	return pdf_specular_reflection(wo, wi);
}

float pdf_mirror_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (has(include, scattering_type(scattering_reflection | scattering_specular)))
		return pdf_specular_reflection(wo, wi);

	return 0;
}

scattering_sample sample_mirror_material(mirror_material material, float3 wo, float2 value)
{
	return sample_specular_reflection(material.reflectance, wo, value);
}

scattering_sample sample_mirror_material(material_shader_buffer material, float3 wo, float2 value, scattering_type include = scattering_all)
{
	scattering_sample sample;
	
	if (!has(include, scattering_type(scattering_reflection | scattering_specular))) {
		sample.type = scattering_unknown;
		sample.pdf = 0;

		return sample;
	}

	return sample_specular_reflection(material.reflectance, wo, value);
}

bool sample_mirror_material_and_lighting(inout ray_payload payload)
{
	mirror_material material = convert_mirror_material(global_materials[global_entities[payload.index].material], payload.interaction.uv);

	payload.value = 0;

	float3 wo = world_to_local(payload.interaction.shading_space, payload.interaction.wo);

	scattering_sample function_sample = sample_mirror_material(material, wo, next_sample2d(payload.sampler));

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