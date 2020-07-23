#ifndef __SHAPE_SAMPLING_HLSL__
#define __SHAPE_SAMPLING_HLSL__

#include "shape_include.hlsl"
#include "../resource_scene.hlsl"

shape_sample sample_triangles(uint index, float2 value)
{
	uint shape_index = global_entities[index].shape;
	uint triangle_count = global_shapes[shape_index].indices;
	
	uint which = min(floor(value.x * triangle_count), triangle_count - 1);

	float2 value_remapped = float2(min(value.x * triangle_count - which, 1 - EPSILON), value.y);

	float2 b = uniform_sample_triangle(value_remapped);

	uint3 primitive_index = global_indices[shape_index][which];

	shape_sample sample;

	float3 position0 = global_positions[shape_index][primitive_index.x];
	float3 position1 = global_positions[shape_index][primitive_index.y];
	float3 position2 = global_positions[shape_index][primitive_index.z];
	
	sample.interaction.position = position0 * b.x + position1 * b.y + position2 * (1 - b.x - b.y);

	if (global_shapes[shape_index].normals != 0)
		sample.interaction.normal =
		global_normals[shape_index][primitive_index.x] * b.x +
		global_normals[shape_index][primitive_index.y] * b.y +
		global_normals[shape_index][primitive_index.z] * (1 - b.x - b.y);
	else
		sample.interaction.normal = normalize(cross(position1 - position0, position2 - position0));

	if (global_shapes[shape_index].reverse != 0)
		sample.interaction.normal *= -1;
	
	float4x4 local_to_world = global_entities[index].local_to_world;
	
	sample.interaction.position = mul(float4(sample.interaction.position, 1), local_to_world).xyz;
	sample.interaction.normal = normalize(mul(float4(sample.interaction.normal, 0), transpose(global_entities[index].world_to_local)).xyz);

	sample.pdf = 1 / global_entities[index].area;

	return sample;
}

shape_sample sample_triangles_with_reference(uint index, interaction reference, float2 value)
{
	shape_sample sample = sample_triangles(index, value);

	float3 wi = sample.interaction.position - reference.position;

	if (length_squared(wi) == 0) { sample.pdf = 0; return sample; }

	wi = normalize(wi);

	sample.pdf = sample.pdf * distance_squared(reference.position, sample.interaction.position) / 
		abs(dot(sample.interaction.normal, -wi));

	if (isnan(sample.pdf)) { sample.pdf = 0; return sample; }

	return sample;
}

float pdf_triangles_with_reference(uint index, interaction reference, float3 wi)
{
	ray_desc ray = reference.spawn_ray(wi);
	ray_payload payload;
	
	payload.missed = false;

	TraceRay(global_acceleration, RAY_FLAG_FORCE_OPAQUE, 0xff, 0,
		1, 0, ray, payload);

	if (payload.missed == true) return 0;

	float pdf = distance_squared(reference.position, payload.interaction.position) /
		(abs(dot(payload.interaction.normal, -wi)) * global_entities[index].area);

	if (isinf(pdf)) return 0;

	return pdf;
}

#endif