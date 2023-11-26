#ifndef __MATERIAL_HLSL__
#define __MATERIAL_HLSL__

#include "../types.hlsl"

struct material_sample
{
    float3 value;
    float3 wi;
    float pdf;
};

#endif
