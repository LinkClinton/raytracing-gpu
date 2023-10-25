#ifndef __TYPES_HLSL__
#define __TYPES_HLSL__


#define SHADER_RESOURCE_DEFINE(type, name, base, space) type name : register(base, space)

// fix resharper c++ error or miss type
#ifndef __HLSL_SHADER__

enum COMMITTED_STATUS : uint
{
    COMMITTED_NOTHING,
    COMMITTED_TRIANGLE_HIT,
    COMMITTED_PROCEDURAL_PRIMITIVE_HIT
};

struct RayQuery
{
    void TraceRayInline(
		RaytracingAccelerationStructure AccelerationStructure,
		uint RayFlags,
		uint InstanceInclusionMask,
		RayDesc Ray);

    void Proceed();

    COMMITTED_STATUS CommittedStatus();

    float CommittedRayT();
};

#define RAY_QUERY(flags) RayQuery

#else

#define RAY_QUERY(flags) RayQuery<flags>

#endif

#endif