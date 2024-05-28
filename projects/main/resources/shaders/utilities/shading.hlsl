#ifndef __SHADING_HLSL__
#define __SHADING_HLSL__

bool in_same_hemisphere(float3 wo, float3 wi)
{
    return wo.z * wi.z > 0;
}

#endif