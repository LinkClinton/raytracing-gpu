#include "resource_scene.hlsl"

[shader("miss")]
void miss_shader(inout SYSTEM_VALUE(ray_payload, payload, SV_RayPayload)) {
	payload.missed = true;
}