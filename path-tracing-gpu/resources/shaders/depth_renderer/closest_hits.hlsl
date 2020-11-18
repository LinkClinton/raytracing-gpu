#include "depth_types.hlsl"

[shader("closesthit")]
void closest_hit(inout tracing_payload payload, HitAttributes attributes)
{
	float3 position = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
	
	payload.missed = 0;
	payload.depth = length(config.camera_position - position) / 10;
}