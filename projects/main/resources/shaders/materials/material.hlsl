#ifndef __MATERIAL_HLSL__
#define __MATERIAL_HLSL__

#include "../resources/dynamic_texture_heap.hlsl"

struct material_sample
{
    float3 value;
    float3 wi;
    float pdf;
};

struct material_create_context
{
    StructuredBuffer<texture_info> package;
    SamplerState sampler;

    float3 sample(uint index, float2 uv);
};

struct material_create_info
{   
    uint index;
    uint type;

    float2 uv;
};

float3 material_create_context::sample(uint index, float2 uv)
{
    return package[index].scale * dynamic_texture_heap::sample_level<float3>(package[index].index, sampler, uv, 0);
}

#endif
