#include "depth_types.hlsl"

[shader("closesthit")]
void closest_hit(inout tracing_payload payload, HitAttributes attributes)
{
	payload.missed = 0;
	payload.depth = 1;
}