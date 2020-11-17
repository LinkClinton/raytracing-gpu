#pragma once

#include <directx12-wrapper/raytracings/accleration.hpp>
#include <directx12-wrapper/raytracings/pipeline.hpp>

#include "renderer.hpp"

namespace path_tracing::renderers {

	struct depth_renderer_scene_config {
		matrix4x4 raster_to_camera = matrix4x4(1);
		matrix4x4 camera_to_world = matrix4x4(1);

		uint32 sample_index = 0;

		vector3 unused = vector3(0);

		depth_renderer_scene_config() = default;
	};
	
	class depth_renderer final : public renderer {
	public:
		depth_renderer(const runtime_service& service);

		~depth_renderer() = default;

		void update(const runtime_service& service, real delta) override;

		void render(const runtime_service& service, real delta) override;
	private:	
		void build_acceleration(const runtime_service& service);

		void build_shader_libraries(const runtime_service& service);
		
		void build_hit_groups(const runtime_service& service);

		void build_descriptor_heap(const runtime_service& service);
		
		void build_root_signature(const runtime_service& service);
		
		void build_shader_association(const runtime_service& service);

		void build_raytracing_pipeline(const runtime_service& service);

		void build_raytracing_shader_table(const runtime_service& service);
		
		wrapper::directx12::command_allocator mCommandAllocator;
		wrapper::directx12::graphics_command_list mCommandList;

		wrapper::directx12::descriptor_table mDescriptorTable;
		wrapper::directx12::descriptor_heap mDescriptorHeap;
		
		wrapper::directx12::root_signature_info mRootSignatureInfo[2];
		wrapper::directx12::root_signature mRootSignature[2];
		
		wrapper::directx12::raytracing_acceleration mAcceleration;

		wrapper::directx12::raytracing_pipeline_info mRaytracingPipelineInfo;
		wrapper::directx12::raytracing_pipeline mRaytracingPipeline;

		wrapper::directx12::raytracing_shader_table mRaytracingShaderTable;

		wrapper::directx12::buffer mRaytracingShaderTableGpuBuffer;
		wrapper::directx12::buffer mRaytracingShaderTableCpuBuffer;

		wrapper::directx12::buffer mSceneConfigCpuBuffer;

		depth_renderer_scene_config mSceneConfig;
	};
	
}
