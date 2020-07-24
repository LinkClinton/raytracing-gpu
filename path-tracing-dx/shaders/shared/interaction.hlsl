#ifndef __INTERACTION_HLSL__
#define __INTERACTION_HLSL__

#include "../utility.hlsl"

struct coordinate_system {
	float3 axes[3];

	float3 x() { return axes[0]; }
	float3 y() { return axes[1]; }
	float3 z() { return axes[2]; }
};

ray_desc spawn_ray(float3 position, float3 direction)
{
	ray_desc ray;

	ray.Direction = direction;
	ray.Origin = position;
	ray.TMin = RAY_EPSILON * (1 + max_component(abs(position)));
	ray.TMax = RAY_INFINITY;

	return ray;
}

ray_desc spawn_ray_to(float3 position, float3 to)
{
	float3 direction = to - position;
	float dist = length(direction);

	direction /= dist;

	ray_desc ray;

	ray.Direction = direction;
	ray.Origin = position;
	ray.TMin = RAY_EPSILON* (1 + max_component(abs(position)));
	ray.TMax = dist * (1 - SHADOW_EPSILON);

	return ray;
}

struct interaction {
	float3 position;
	float3 normal; 
	float3 wo;

	ray_desc spawn_ray_to(interaction interaction)
	{
		return ::spawn_ray_to(position, interaction.position);
	}

	ray_desc spawn_ray(float3 direction)
	{
		return ::spawn_ray(position, direction);
	}
};

struct surface_interaction {
	coordinate_system shading_space;

	float3 position;
	float3 normal;
	float3 wo;

	float2 uv;

	ray_desc spawn_ray_to(interaction interaction)
	{
		return ::spawn_ray_to(position, interaction.position);
	}

	ray_desc spawn_ray(float3 direction)
	{
		return ::spawn_ray(position, direction);
	}

	interaction base_type()
	{
		interaction base;

		base.position = position;
		base.normal = normal;
		base.wo = wo;

		return base;
	}
};

float3 local_to_world(coordinate_system system, float3 value) 
{
	return float3(
		system.x().x * value.x + system.y().x * value.y + system.z().x * value.z,
		system.x().y * value.x + system.y().y * value.y + system.z().y * value.z,
		system.x().z * value.x + system.y().z * value.y + system.z().z * value.z);
}

float3 world_to_local(coordinate_system system, float3 value)
{
	return float3(dot(value, system.x()), dot(value, system.y()), dot(value, system.z()));
}

coordinate_system build_coordinate_system(float3 normal)
{
	coordinate_system system;

	system.axes[2] = normalize(normal);

	float3 z = system.z();

	system.axes[0] = abs(z.x) > abs(z.y) ? 
		float3(-z.z, 0, z.x) / sqrt(z.x * z.x + z.z * z.z): 
		float3(0, z.z, -z.y) / sqrt(z.y * z.y + z.z * z.z);

	system.axes[1] = cross(z, system.x());

	return system;
}

#endif