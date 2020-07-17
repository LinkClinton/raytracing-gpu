#include "resource_scene.hlsl"

[shader("miss")]
void miss_shader(inout ray_payload payload: SV_RayPayload) {
	payload.missed = true;
}