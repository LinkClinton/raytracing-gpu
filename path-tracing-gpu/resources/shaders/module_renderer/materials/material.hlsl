#ifndef __MATERIAL_HLSL__
#define __MATERIAL_HLSL__

#include "../../scatterings/scattering.hlsl"

// only used for coding, when we compile the ray_generation shader
// we will define __HLSL_SHADER__ and write the real material structure
// and real sample_material, evaluate_material, pdf_material, unpacking material
#ifndef __HLSL_SHADER__

struct packed_material {};

struct material {
	float3 diffuse;
	uint type;
};

material unpacking_material(packed_material m, float2 uv)
{
	return material();
}

float3 evaluate_material(material material, float3 wo, float3 wi)
{
	return float3(0, 0, 0);
}

real pdf_material(material material, float3 wo, float3 wi)
{
	return 0;
}

scattering_sample sample_material(material material, float3 wo, float2 value)
{
	return scattering_sample();
}

SHADER_STRUCTURED_BUFFER_DEFINE(packed_material, materials, t2, space3);

#endif

#endif