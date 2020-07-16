struct scene_info {
	float4x4 raster_to_camera;
	float4x4 camera_to_world;
	float4x4 unused1;
	float4x4 unused2;
};

enum material_type {
	material_unknown = 0,
	material_diffuse = 1
};

enum emitter_type {
	emitter_unknown = 0,
	emitter_point = 1
};

struct material_gpu_buffer {
	material_type type;
	float3 diffuse;
};

struct emitter_gpu_buffer {
	emitter_type type;
	float3 intensity;

	float3 position;
	float unused;
};

struct ray_payload {
	float3 spectrum;
};

typedef BuiltInTriangleIntersectionAttributes HitAttributes;

ConstantBuffer<scene_info> global_scene_info : register(b0, space0);
RaytracingAccelerationStructure global_acceleration : register(t0, space1);
StructuredBuffer<material_gpu_buffer> global_materials : register(t1, space1);
StructuredBuffer<emitter_gpu_buffer> global_emitters : register(t2, space1);

RWTexture2D<float4> global_render_target : register(u0, space2);