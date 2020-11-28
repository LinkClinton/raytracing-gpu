#include "../shared/interaction.hlsl"
#include "../types.hlsl"

#include "module_types.hlsl"

struct hit_group_info {
	uint index;
};

SHADER_CONSTANT_BUFFER_DEFINE(hit_group_info, local_info, b0, space100);

surface_interaction build_surface_interaction(HitAttributes attribute)
{
	surface_interaction interaction;

	mesh_info mesh = entities[local_info.index].mesh;

	uint3 index = indices[PrimitiveIndex() + mesh.idx_location] + mesh.vtx_location;

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

	float4x3 local_to_world = ObjectToWorld4x3();
	float3x3 inv_transpose = float3x3(WorldToObject4x3()[0], WorldToObject4x3()[1], WorldToObject4x3()[2]);

	interaction.position = mul(float4(local_position, 1), local_to_world).xyz;
	interaction.normal = normalize(mul(inv_transpose, mesh.reverse != 0 ? local_normal * -1 : local_normal).xyz);
	interaction.uv = (uvs[index.x] * b0 + uvs[index.y] * b1 + uvs[index.z] * b2).xy;
	interaction.wo = -WorldRayDirection();

	float3 shading_normal =
		mesh.normals != 0 ? normals[index.x] * b0 + normals[index.y] * b1 + normals[index.z] * b2 : local_normal;

	if (mesh.reverse != 0) shading_normal *= -1;

	interaction.shading_space = build_coordinate_system(mul(inv_transpose, shading_normal).xyz);
	interaction.normal = face_forward(interaction.normal, interaction.shading_space.z());

	return interaction;
}

[shader("closesthit")]
void closest_hit(inout tracing_payload payload, HitAttributes attribute)
{
	payload.interaction = build_surface_interaction(attribute);
	payload.entity = local_info.index;
	payload.missed = false;
}
