#ifndef __MATERIAL_UBER_HLSL__
#define __MATERIAL_UBER_HLSL__

#include "../scatterings/scattering_lambertian.hlsl"
#include "../scatterings/scattering_microfacet.hlsl"

#include "material_include.hlsl"

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

scattering_sample sample_uber_material(material_shader_buffer material, float3 wo, float2 value, scattering_type include = scattering_all)
{
	scattering_sample sample;

	// todo : remove include
	// sample first bsdf
	if (value.x < 0.5) {
		value.x = min(value.x * 2 - 0, 1 - EPSILON);

		sample.type = scattering_type(scattering_reflection | scattering_diffuse);
		sample.wi = sample_lambertian_reflection(material.diffuse, wo, value).wi;
	}else {
		value.x = min(value.x * 2 - 1, 1 - EPSILON);

		sample.type = scattering_type(scattering_reflection | scattering_glossy);
		sample.wi = sample_dielectric_microfacet_reflection(material.specular, material.eta.x,
			material.roughness_u, material.roughness_v, wo, value).wi;
	}

	sample.value = evaluate_uber_material(material, wo, sample.wi, include);
	sample.pdf = pdf_uber_material(material, wo, sample.wi, include);

	return sample;
}

#endif