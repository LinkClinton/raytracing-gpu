#ifndef __MATERIAL_HLSL__
#define __MATERIAL_HLSL__

#include "../textures/texture.hlsl"
#include "material_include.hlsl"

#include "material_substrate.hlsl"
#include "material_diffuse.hlsl"
#include "material_mirror.hlsl"

material_shader_buffer convert_gpu_buffer_to_shader_buffer(material_gpu_buffer gpu_buffer, float2 value)
{
	material_shader_buffer shader_buffer;

	shader_buffer.type = gpu_buffer.type;
	shader_buffer.reflectance = gpu_buffer.reflectance != ENTITY_NUll ? sample_texture(global_textures[gpu_buffer.reflectance], value) : 0;
	shader_buffer.specular = gpu_buffer.specular != ENTITY_NUll ? sample_texture(global_textures[gpu_buffer.specular], value) : 0;
	shader_buffer.diffuse = gpu_buffer.diffuse != ENTITY_NUll ? sample_texture(global_textures[gpu_buffer.diffuse], value) : 0;
	shader_buffer.roughness_u = gpu_buffer.roughness_u != ENTITY_NUll ? sample_texture(global_textures[gpu_buffer.roughness_u], value).x : 0;
	shader_buffer.roughness_v = gpu_buffer.roughness_v != ENTITY_NUll ? sample_texture(global_textures[gpu_buffer.roughness_v], value).x : 0;

	if (gpu_buffer.remapped != 0) {
		shader_buffer.roughness_u = roughness_to_alpha(shader_buffer.roughness_u);
		shader_buffer.roughness_v = roughness_to_alpha(shader_buffer.roughness_v);
	}
		
	return shader_buffer;
}

float3 evaluate_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	switch (material.type) {
	case material_substrate: return evaluate_substrate_material(material, wo, wi, include);
	case material_diffuse: return evaluate_diffuse_material(material, wo, wi, include);
	case material_mirror: return evaluate_mirror_material(material, wo, wi, include);
	default:
		return 0;
	}
}

float pdf_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	switch (material.type) {
	case material_substrate: return pdf_substrate_material(material, wo, wi, include);
	case material_diffuse: return pdf_diffuse_material(material, wo, wi, include);
	case material_mirror: return pdf_mirror_material(material, wo, wi, include);
	default:
		return 0;
	}
}

scattering_sample sample_material(material_shader_buffer material, float3 wo, float2 value, scattering_type include = scattering_all)
{
	scattering_sample sample; sample.pdf = 0; sample.type = scattering_unknown;

	switch (material.type) {
	case material_substrate: return sample_substrate_material(material, wo, value, include);
	case material_diffuse: return sample_diffuse_material(material, wo, value, include);
	case material_mirror: return sample_mirror_material(material, wo, value, include);
	default: return sample;
	}
}

#endif