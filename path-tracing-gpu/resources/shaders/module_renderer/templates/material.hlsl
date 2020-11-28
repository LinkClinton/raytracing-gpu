#ifndef __HLSL_SHADER__
#include "../../scatterings/scattering.hlsl"
#include "../../types.hlsl"
#endif

struct packed_material {
	// [packed_material]	
	// [packed_material]
};

struct material {
	// [common_material]
	// [common_material]
	
#ifndef __HLSL_SHADER__
	uint type;
#endif
};

SHADER_STRUCTURED_BUFFER_DEFINE(packed_material, materials, t2, space3);

// [include_material_submodule]
// [include_material_submodule]

material unpacking_material(packed_material packed_material, float2 uv)
{
	material material;

	// [unpacking_material]
	// [unpacking_material]
	
	return material;
}

float3 evaluate_material(material material, float3 wo, float3 wi)
{
	switch (material.type) {
	// [evaluate_material]
	// [evaluate_material]
	default: return 0;
	}
}

scattering_sample sample_material(material material, float3 wo, float2 value)
{
	scattering_sample sample; sample.type = scattering_unknown; sample.pdf = 0;

	switch (material.type) {
		// [sample_material]
		// [sample_material]
	default: return sample;
	}
}

real pdf_material(material material, float3 wo, float3 wi)
{
	switch (material.type) {
	// [pdf_material]
	// [pdf_material]
	default: return 0;
	}
}