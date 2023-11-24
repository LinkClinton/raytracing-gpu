#pragma once

#include "renderer.hpp"

namespace raytracing::renderers
{
	struct shade_renderer_frame_data final
	{
		matrix4x4 raster_to_camera = matrix4x4(1);
		matrix4x4 camera_to_world = matrix4x4(1);

		vector3 camera_position = vector3(0);

		uint32 sample_index = 0;
	};

	struct shade_renderer_entity_data final
	{
		uint32 geometry_index = index_null;
		uint32 material_index = index_null;
		uint32 light_index = index_null;

		uint32 material_type = index_null;
		uint32 light_type = index_null;
	};
	
	class shade_renderer final : public renderer
	{
	public:
		explicit shade_renderer(const runtime_service& service);

		void update(const runtime_service& service, const runtime_frame& frame) override;

		void render(const runtime_service& service, const runtime_frame& frame) override;
	private:
		using frame_data = shade_renderer_frame_data;
		using entity_data = shade_renderer_entity_data;

		wrapper::directx12::command_allocator mCommandAllocator;
		wrapper::directx12::graphics_command_list mCommandList;

		wrapper::directx12::shader_library mShaderLibrary;

		wrapper::directx12::raytracing_acceleration mAcceleration;
		wrapper::directx12::raytracing_shader_table mShaderTable;
		wrapper::directx12::raytracing_pipeline mPipeline;

		wrapper::directx12::descriptor_table mDescriptorTable;
		wrapper::directx12::descriptor_heap mDescriptorHeap;

		wrapper::directx12::root_signature_info mRootSignatureInfo;
		wrapper::directx12::root_signature mRootSignature;

		wrapper::directx12::buffer mShaderTableCpuBuffer;
		wrapper::directx12::buffer mShaderTableGpuBuffer;
		wrapper::directx12::buffer mEntitiesDataCpuBuffer;
		wrapper::directx12::buffer mEntitiesDataGpuBuffer;
		wrapper::directx12::buffer mFrameDataCpuBuffer;

		std::vector<wrapper::directx12::buffer> mMaterialCpuBuffers;
		std::vector<wrapper::directx12::buffer> mMaterialGpuBuffers;
		std::vector<wrapper::directx12::buffer> mLightCpuBuffers;
		std::vector<wrapper::directx12::buffer> mLightGpuBuffers;

		frame_data mFrameData;
	};
}
