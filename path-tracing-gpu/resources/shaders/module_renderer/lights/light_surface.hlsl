#ifndef __LIGHT_SURFACE_HLSL__
#define __LIGHT_SURFACE_HLSL__

#include "../module_types.hlsl"

#include "light.hlsl"

struct triangle_sample {
	interaction interaction;
	real pdf;
};

triangle_sample sample_triangles(uint entity, float2 value)
{
	entity_info info = entities[entity];
	
	uint which = min(floor(value.x * info.mesh.triangle_count), info.mesh.triangle_count - 1);

	float2 value_remapped = float2(min(value.x * info.mesh.triangle_count - which, 1 - EPSILON), value.y);

	float2 b = uniform_sample_triangle(value_remapped);

	uint3 primitive_index = indices[info.mesh.idx_location + which] + info.mesh.vtx_location;

	triangle_sample sample;
	
	float3 position0 = positions[primitive_index.x];
	float3 position1 = positions[primitive_index.y];
	float3 position2 = positions[primitive_index.z];

	sample.interaction.position = position0 * b.x + position1 * b.y + position2 * (1 - b.x - b.y);

	if (info.mesh.normals != 0)
		sample.interaction.normal = 
			normals[primitive_index.x] * b.x + 
			normals[primitive_index.y] * b.y + 
			normals[primitive_index.z] * (1 - b.x - b.y);
	else
		sample.interaction.normal = normalize(cross(position1 - position0, position2 - position0));

	if (info.mesh.reverse != 0)
		sample.interaction.normal *= -1;

	sample.interaction.position = mul(float4(sample.interaction.position, 1), info.local_to_world).xyz;
	sample.interaction.normal = normalize(mul(float4(sample.interaction.normal, 0), transpose(info.world_to_local)).xyz);

	sample.pdf = 1 / info.area;

	return sample;
}

triangle_sample sample_triangles(uint entity, interaction reference, float2 value)
{
	triangle_sample sample = sample_triangles(entity, value);

	float3 wi = sample.interaction.position - reference.position;

	if (length_squared(wi) == 0) { sample.pdf = 0; return sample; }

	wi = normalize(wi);

	sample.pdf = sample.pdf * distance_squared(reference.position, sample.interaction.position) /
		abs(dot(sample.interaction.normal, -wi));

	if (isnan(sample.pdf)) { sample.pdf = 0; return sample; }

	return sample;
}

real pdf_triangles_with_reference(uint entity, interaction reference, float3 wi)
{
	ray_desc ray = reference.spawn_ray(wi);
	tracing_payload payload;

	payload.missed = 0;

	TraceRay(acceleration, RAY_FLAG_FORCE_OPAQUE, 0xff, 0,
		1, 0, ray, payload);

	if (payload.missed != 0) return 0;

	real pdf = distance_squared(reference.position, payload.interaction.position) /
		(abs(dot(payload.interaction.normal, -wi)) * entities[entity].area);

	if (isinf(pdf)) return 0;

	return pdf;
}

float3 evaluate_surface_light(light light, interaction interaction, float3 wi)
{
	return (dot(interaction.normal, wi) > 0) ? light.intensity : 0;
}

light_sample sample_surface_light(light light, interaction reference, float2 value)
{
	light_sample sample;

	triangle_sample triangle_sample = sample_triangles(light.entity, reference, value);

	if (triangle_sample.pdf == 0) { sample.pdf = 0; return sample; }

	sample.wi = normalize(triangle_sample.interaction.position - reference.position);
	sample.interaction = triangle_sample.interaction;
	sample.intensity = evaluate_surface_light(light, triangle_sample.interaction, -sample.wi);
	sample.pdf = triangle_sample.pdf;

	return sample;
}

real pdf_surface_light(light light, interaction reference, float3 wi)
{
	return pdf_triangles_with_reference(light.entity, reference, wi);
}

#endif