#ifndef __MATERIAL_MIRROR_HLSL__
#define __MATERIAL_MIRROR_HLSL__

#include "../scatterings/scattering_specular.hlsl"
#include "material_include.hlsl"

float3 evaluate_mirror_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (wo.z == 0 || same_hemisphere(wo, wi) == false) return 0;

	if (has(include, scattering_type(scattering_reflection | scattering_specular)))
		return evaluate_specular_reflection(material.reflectance, wo, wi);

	return 0;
}

float pdf_mirror_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (has(include, scattering_type(scattering_reflection | scattering_specular)))
		return pdf_specular_reflection(wo, wi);

	return 0;
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

#endif