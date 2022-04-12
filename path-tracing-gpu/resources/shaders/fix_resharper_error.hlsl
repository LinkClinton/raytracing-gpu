#ifndef __FIX_RESHARPER_ERROR__
#define __FIX_RESHARPER_ERROR__

#ifndef __HLSL_SHADER__

void TraceRay(
	RaytracingAccelerationStructure AccelerationStructure,
	uint RayFlags,
	uint InstanceInclusionMask,
	uint RayContributionToHitGroupIndex,
	uint MultiplierForGeometryContributionToShaderIndex,
	uint MissShaderIndex,
	RayDesc ray,
	inout Payload payload) {}

#else


#endif

#define SHADER_STRUCTURED_BUFFER_DEFINE(type, name, base, space) StructuredBuffer<type> name : register(base, space)
#define SHADER_CONSTANT_BUFFER_DEFINE(type, name, base, space) ConstantBuffer<type> name : register(base, space)
#define SHADER_RESOURCE_DEFINE(type, name, base, space) type name : register(base, space)

#define SYSTEM_VALUE(type, name, value) type name : value

#endif