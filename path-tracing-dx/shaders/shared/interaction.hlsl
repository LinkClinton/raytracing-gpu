#ifndef __INTERACTION_HLSL__
#define __INTERACTION_HLSL__

struct coordinate_system {
	float3 axes[3];

	float3 x() { return axes[0]; }
	float3 y() { return axes[1]; }
	float3 z() { return axes[2]; }
};

struct interaction {
	float3 position;
	float3 normal; 
	float3 wo;
};

struct surface_interaction {
	coordinate_system shading_space;

	float3 position;
	float3 normal;
	float3 wo;

	float2 uv;
};

interaction base_type(surface_interaction input) 
{
	interaction base;

	base.position = input.position;
	base.normal = input.normal;
	base.wo = input.wo;

	return base;
}

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

#endif