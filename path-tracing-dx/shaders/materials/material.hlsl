#ifndef __MATERIAL_HLSL__
#define __MATERIAL_HLSL__

#include "../textures/texture.hlsl"
#include "material_include.hlsl"
#include "material_diffuse.hlsl"
#include "material_mirror.hlsl"

material_shader_buffer convert_gpu_buffer_to_shader_buffer(material_gpu_buffer gpu_buffer, float2 value)
{
	material_shader_buffer shader_buffer;

	shader_buffer.type = gpu_buffer.type;
	shader_buffer.reflectance = gpu_buffer.reflectance != ENTITY_NUll ? sample_texture(global_textures[gpu_buffer.reflectance], value) : 0;
	shader_buffer.diffuse = gpu_buffer.diffuse != ENTITY_NUll ? sample_texture(global_textures[gpu_buffer.diffuse], value) : 0;

	return shader_buffer;
}

float3 evaluate_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (material.type == material_diffuse)
		return evaluate_diffuse_material(material, wo, wi, include);

	if (material.type == material_mirror)
		return evaluate_mirror_material(material, wo, wi, include);
	
	return 0;
}

float pdf_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (material.type == material_diffuse)
		return pdf_diffuse_material(material, wo, wi, include);

	if (material.type == material_mirror)
		return pdf_mirror_material(material, wo, wi, include);

	return 0;
}

scattering_sample sample_material(material_shader_buffer material, float3 wo, float2 value, scattering_type include = scattering_all)
{
	if (material.type == material_diffuse)
		return sample_diffuse_material(material, wo, value, include);

	if (material.type == material_mirror)
		return sample_mirror_material(material, wo, value, include);
	
	scattering_sample sample; sample.pdf = 0; sample.type = scattering_unknown;

	return sample;
}

#endif