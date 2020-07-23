#ifndef __MATERIAL_SUBSTRATE_HLSL__
#define __MATERIAL_SUBSTRATE_HLSL__

#include "material_include.hlsl"
#include "../scatterings/scattering_fresnel_blend.hlsl"

float3 evaluate_substrate_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (wo.z == 0 || same_hemisphere(wo, wi) == false) return 0;

	if (has(include, scattering_type(scattering_reflection | scattering_glossy)))
		return evaluate_fresnel_blend_reflection(material.specular, material.diffuse, 
			material.roughness_u, material.roughness_v, wo, wi);

	return 0;
}

float pdf_substrate_material(material_shader_buffer material, float3 wo, float3 wi, scattering_type include = scattering_all)
{
	if (has(include, scattering_type(scattering_reflection | scattering_glossy)))
		return pdf_fresnel_blend_reflection(material.roughness_u, material.roughness_v, wo, wi);

	return 0;
}

scattering_sample sample_substrate_material(material_shader_buffer material, float3 wo, float2 value, scattering_type include = scattering_all)
{
	if (!has(include, scattering_type(scattering_reflection | scattering_glossy)) || wo.z == 0) {
		scattering_sample sample;

		sample.type = scattering_unknown;
		sample.pdf = 0;

		return sample;
	}

	return sample_fresnel_blend_reflection(material.specular, material.diffuse, material.roughness_u, material.roughness_v, wo, value);
}


#endif