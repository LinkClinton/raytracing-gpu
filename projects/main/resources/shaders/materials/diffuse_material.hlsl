#ifndef __DIFFUSE_MATERIAL_HLSL__
#define __DIFFUSE_MATERIAL_HLSL__

#include "material.hlsl"

struct diffuse_material_metadata
{
    static const uint count = 1;
    static const uint type = 0;

    static const uint diffuse = 0;
};

struct diffuse_material
{
    float3 diffuse;

    float3 evaluate(float3 wo, float3 wi);

	float pdf(float3 wo, float3 wi);

    material_sample sample(float3 wo, float2 value);

    static diffuse_material create(
		material_create_context context, 
		material_create_info info);
};

float3 diffuse_material::evaluate(float3 wo, float3 wi)
{
    return in_same_hemisphere(wo, wi) ? diffuse * ONE_OVER_PI : 0;
}

float diffuse_material::pdf(float3 wo, float3 wi)
{
    return sampling_uniform_hemisphere::pdf();
}

material_sample diffuse_material::sample(float3 wo, float2 value)
{
    material_sample sample;

    sample.wi = sampling_uniform_hemisphere::get(value);
    sample.pdf = sampling_uniform_hemisphere::pdf();
    sample.value = evaluate(wo, sample.wi);

    return sample;
}

diffuse_material diffuse_material::create(
	material_create_context context,
	material_create_info info)
{
    diffuse_material material;

    material.diffuse = context.sample(info.index + 0, info.uv);

    return material;
}

#endif
