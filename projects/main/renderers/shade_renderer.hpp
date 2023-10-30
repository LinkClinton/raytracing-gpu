#pragma once

#include "renderer.hpp"

namespace raytracing::renderers
{
	struct shade_renderer_frame_data final
	{
		
	};

	class shade_renderer final : public renderer
	{
	public:
		explicit shade_renderer(const runtime_service& service);

		void update(const runtime_service& service, const runtime_frame& frame) override;

		void render(const runtime_service& service, const runtime_frame& frame) override;
	private:
		using frame_data = shade_renderer_frame_data;

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

		wrapper::directx12::buffer mShaderTableGpuBuffer;
		wrapper::directx12::buffer mFrameDataCpuBuffer;

		frame_data mFrameData;
	};
}
