#ifndef __MATERIAL_MIRROR_HLSL__
#define __MATERIAL_MIRROR_HLSL__

#include "material.hlsl"

float3 evaluate_mirror_material(material material, float3 wo, float3 wi)
{
	return 0;
}

float pdf_mirror_material(material material, float3 wo, float3 wi)
{
	return 0;
}

scattering_sample sample_mirror_material(material material, float3 wo, float2 value)
{
	const float3 wi = float3(-wo.x, -wo.y, wo.z);

	scattering_sample sample;

	sample.type = scattering_type(scattering_reflection | scattering_specular);
	sample.value = material.reflectance / abs(cos_theta(wi));
	sample.wi = wi;
	sample.pdf = 1;

	return sample;
}

#endif