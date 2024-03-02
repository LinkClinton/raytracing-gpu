#ifndef __MATERIAL_WRAPPER_HLSL__
#define __MATERIAL_WRAPPER_HLSL__

#include "diffuse_material.hlsl"

struct material_wrapper_metadata
{
    static const uint count = MAX(diffuse_material_metadata::count, 1);
    static const uint count_per_type[count];
};

const uint material_wrapper_metadata::count_per_type[material_wrapper_metadata::count] = 
{
    diffuse_material_metadata::count
};

struct material_wrapper
{
    float3 value[material_wrapper_metadata::count];

	uint type;

    float3 evaluate(float3 wo, float3 wi);

    float pdf(float3 wo, float3 wi);

    material_sample sample(float3 wo, float2 value);

    diffuse_material diffuse();

    static material_wrapper create(
		material_create_context context,
		material_create_info info);

    static material_wrapper create(diffuse_material diffuse);
};

float3 material_wrapper::evaluate(float3 wo, float3 wi)
{
    switch (type)
    {
        case diffuse_material_metadata::type:
            return diffuse().evaluate(wo, wi);
        default:
            return 0;
    }
}

float material_wrapper::pdf(float3 wo, float3 wi)
{
    switch (type)
    {
        case diffuse_material_metadata::type:
            return diffuse().pdf(wo, wi);
		default:
            return 0;
    }
}

material_sample material_wrapper::sample(float3 wo, float2 value)
{
    switch (type)
    {
        case diffuse_material_metadata::type:
            return diffuse().sample(wo, value);
		default:
            return (material_sample)0;
    }
}

diffuse_material material_wrapper::diffuse()
{
    diffuse_material material;

    material.diffuse = value[diffuse_material_metadata::diffuse];

    return material;
}

material_wrapper material_wrapper::create(
	material_create_context context,
	material_create_info info)
{
    material_wrapper wrapper;

    wrapper.type = info.type;

    for (uint index = 0; index < material_wrapper_metadata::count_per_type[wrapper.type]; index++)
    {
        wrapper.value[index] = context.sample(info.index + index, info.uv);
    }

    return wrapper;
}

material_wrapper
    material_wrapper::create(diffuse_material diffuse)
{
    material_wrapper wrapper;

    wrapper.type = diffuse_material_metadata::type;
    wrapper.value[diffuse_material_metadata::diffuse] = diffuse.diffuse;

    return wrapper;
}

#endif
