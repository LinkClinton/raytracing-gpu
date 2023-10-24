#ifndef __TYPES_HLSL__
#define __TYPES_HLSL__


#define SHADER_RESOURCE_DEFINE(type, name, base, space) type name : register(base, space)

// fix resharper c++ error or miss type
#ifndef __HLSL_SHADER__

struct RayQuery
{
    void TraceRayInline(
		RaytracingAccelerationStructure AccelerationStructure,
		uint RayFlags,
		uint InstanceInclusionMask,
		RayDesc Ray);
};

#define RAY_QUERY(flags) RayQuery

#else

#define RAY_QUERY(flags) RayQuery<flags>

#endif

#endif