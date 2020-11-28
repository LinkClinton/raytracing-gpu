#pragma once

#include <directx12-wrapper/raytracings/accleration.hpp>
#include <directx12-wrapper/raytracings/pipeline.hpp>
#include <directx12-wrapper/shaders/shader_creator.hpp>

#include "renderer.hpp"

namespace path_tracing::renderers {

	struct module_renderer_scene_config final {
		matrix4x4 raster_to_camera = matrix4x4(1);
		matrix4x4 camera_to_world = matrix4x4(1);

		vector3 camera_position = vector3(0);

		uint32 sample_index = 0;

		uint32 max_depth = 5;
		uint32 lights = 0;
		uint32 unused0 = 0;
		uint32 unused1 = 0;

		module_renderer_scene_config() = default;
	};

	struct module_renderer_mesh_info {
		uint32 triangle_count;

		uint32 vtx_location;
		uint32 idx_location;

		uint32 reverse;
		uint32 normals;
		uint32 uvs;
	};
	
	struct module_renderer_entity_info {
		matrix4x4 local_to_world;
		matrix4x4 world_to_local;

		module_renderer_mesh_info mesh;

		uint32 material;
		uint32 light;
		real area;
	};
	
	struct submodule final {
		std::string file = "";
		std::string name = "";

		submodule() = default;
	};

	class module_renderer : public renderer {
	public:
		module_renderer(
			const std::vector<submodule>& materials,
			const std::vector<submodule>& lights,
			const runtime_service& service);

		~module_renderer() = default;

		void update(const runtime_service& service, const runtime_frame& frame) override;

		void render(const runtime_service& service, const runtime_frame& frame) override;
	private:
		void build_material_submodule(const runtime_service& service, const std::vector<submodule>& materials);

		void build_light_submodule(const runtime_service& service, const std::vector<submodule>& lights);
		
		void build_acceleration(const runtime_service& service);

		void build_shader_libraries(const runtime_service& service);

		void build_hit_groups(const runtime_service& service);

		void build_descriptor_heap(const runtime_service& service);
		
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

		wrapper::directx12::shader_creator mMaterialSubmodule;
		wrapper::directx12::shader_creator mLightSubmodule;
		
		wrapper::directx12::buffer mRaytracingShaderTableGpuBuffer;
		wrapper::directx12::buffer mRaytracingShaderTableCpuBuffer;

		wrapper::directx12::buffer mSceneConfigCpuBuffer;
		
		wrapper::directx12::buffer mEntityInfoCpuBuffer;
		wrapper::directx12::buffer mEntityInfoGpuBuffer;
		
		wrapper::directx12::buffer mMaterialCpuBuffer;
		wrapper::directx12::buffer mMaterialGpuBuffer;

		wrapper::directx12::buffer mLightCpuBuffer;
		wrapper::directx12::buffer mLightGpuBuffer;

		mapping<std::string, uint32> mMaterialValuesMemoryAddress;
		mapping<std::string, uint32> mLightValuesMemoryAddress;
		
		mapping<std::string, uint32> mMaterialTypes;
		mapping<std::string, uint32> mLightTypes;
		
		module_renderer_scene_config mSceneConfig;

		size_t mMaterialSubmoduleTypeSize = 0;
		size_t mLightSubmoduleTypeSize;
	};
	
}
