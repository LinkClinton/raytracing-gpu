#ifndef __MATERIAL_WRAPPER_HLSL__
#define __MATERIAL_WRAPPER_HLSL__

#include "diffuse_material.hlsl"

struct material_wrapper_metadata
{
    static const uint count = MAX(diffuse_material_metadata::count, 1);
};

struct material_wrapper
{
    float3 value[material_wrapper_metadata::count];

	uint identity;

    float3 evaluate(float3 wo, float3 wi);

    float pdf(float3 wo, float3 wi);

    material_sample sample(float3 wo, float2 value);

    diffuse_material diffuse();

    static material_wrapper create(
		StructuredBuffer<texture_handle> package,
		Texture2D<float4> textures[],
		SamplerState texture_sampler,
		uint type, uint index, float2 uv, float lod);

    static material_wrapper create(diffuse_material diffuse);
};

float3 material_wrapper::evaluate(float3 wo, float3 wi)
{
	switch (identity)
	{
        case diffuse_material_metadata::identity:
            return diffuse().evaluate(wo, wi);
        default:
            return 0;
    }
}

float material_wrapper::pdf(float3 wo, float3 wi)
{
	switch (identity)
	{
		case diffuse_material_metadata::identity:
            return diffuse().pdf(wo, wi);
		default:
            return 0;
    }
}

material_sample material_wrapper::sample(float3 wo, float2 value)
{
	switch (identity)
	{
		case diffuse_material_metadata::identity:
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
	StructuredBuffer<texture_handle> package, 
	Texture2D<float4> textures[],
	SamplerState texture_sampler, 
	uint type, uint index, float2 uv, float lod)
{
	switch (type)
	{
		case diffuse_material_metadata::identity:
            return create(diffuse_material::create(package, textures, texture_sampler, index, uv, lod));
		default:
            return (material_wrapper)0;
    }
}

material_wrapper material_wrapper::create(diffuse_material diffuse)
{
    material_wrapper wrapper;

    wrapper.identity = diffuse_material_metadata::identity;
    wrapper.value[diffuse_material_metadata::diffuse] = diffuse.diffuse;

    return wrapper;
}

#endif
