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
    return diffuse * ONE_OVER_PI;
}

float diffuse_material::pdf(float3 wo, float3 wi)
{
    //todo:
    return 0;
}

material_sample diffuse_material::sample(float3 wo, float2 value)
{
    //todo:
    return (material_sample) 0;
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
