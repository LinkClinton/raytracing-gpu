#ifndef __CLOSEST_HIT_HLSL__
#define __CLOSEST_HIT_HLSL__

#include "resource_scene.hlsl"

StructuredBuffer<float3> positions : register(t0, space100);
StructuredBuffer<float3> normals : register(t1, space100);
StructuredBuffer<float3> uvs : register(t3, space100);

StructuredBuffer<uint3> indices : register(t2, space100);

[shader("closesthit")]
void closest_hit_shader(inout ray_payload payload, HitAttributes attribute) {
	uint3 index = indices[PrimitiveIndex()];

	float3 position0 = positions[index.x];
	float3 position1 = positions[index.y];
	float3 position2 = positions[index.z];

	float3 normal0 = normals[index.x];
	float3 normal1 = normals[index.y];
	float3 normal2 = normals[index.z];

	float3 normal = normalize(
		normal0 * (1 - attribute.barycentrics.x - attribute.barycentrics.y) + 
		normal1 * (attribute.barycentrics.x) +
		normal2 * (attribute.barycentrics.y));

	payload.radiance = normal;
}

#endif