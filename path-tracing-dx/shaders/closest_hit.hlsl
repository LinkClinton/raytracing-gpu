#ifndef __CLOSEST_HIT_HLSL__
#define __CLOSEST_HIT_HLSL__

#include "resource_scene.hlsl"

struct hit_group_info {
	uint index; 
};

SHADER_STRUCTURED_BUFFER_DEFINE(float3, positions, t0, space100);
SHADER_STRUCTURED_BUFFER_DEFINE(float3, normals, t1, space100);
SHADER_STRUCTURED_BUFFER_DEFINE(float3, uvs, t3, space100);
SHADER_STRUCTURED_BUFFER_DEFINE(uint3, indices, t2, space100);
SHADER_CONSTANT_BUFFER_DEFINE(hit_group_info, local_group_info, b4, space100);

[shader("closesthit")]
void closest_hit_shader(inout ray_payload payload, HitAttributes attribute) {
	uint3 index = indices[PrimitiveIndex()];
	
	float3 position0 = positions[index.x];
	float3 position1 = positions[index.y];
	float3 position2 = positions[index.z];

	float3 e1 = position1 - position0;
	float3 e2 = position2 - position0;

	float b0 = 1 - attribute.barycentrics.x - attribute.barycentrics.y;
	float b1 = attribute.barycentrics.x;
	float b2 = attribute.barycentrics.y;

	float3 local_position = position0 * b0 + position1 * b1 + position2 * b2;
	float3 local_normal = cross(e1, e2);

	uint reverse = global_shapes[global_entities[local_group_info.index].shape].reverse;

	float4x3 local_to_world = ObjectToWorld4x3();
	float3x3 inv_transpose = float3x3(WorldToObject4x3()[0], WorldToObject4x3()[1], WorldToObject4x3()[2]);
	
	payload.interaction.position = mul(float4(local_position, 1), local_to_world).xyz;
	payload.interaction.normal = normalize(mul(inv_transpose, reverse != 0 ? local_normal * -1 : local_normal).xyz);
	payload.interaction.uv = (uvs[index.x] * b0 + uvs[index.y] * b1 + uvs[index.z] * b2).xy;
	payload.interaction.wo = -WorldRayDirection();

	float3 shading_normal = 
		global_shapes[global_entities[local_group_info.index].shape].normals != 0 ? 
		normals[index.x] * b0 + normals[index.y] * b1 + normals[index.z] * b2 :
		local_normal;

	if (reverse != 0) shading_normal *= -1;

	payload.interaction.shading_space = build_coordinate_system(mul(inv_transpose, shading_normal).xyz);
	payload.index = local_group_info.index;
	payload.missed = false;

	payload.interaction.normal = face_forward(payload.interaction.normal, payload.interaction.shading_space.z());
}

#endif