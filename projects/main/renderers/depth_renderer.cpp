#include "depth_renderer.hpp"

#include <directx12-wrapper/shaders/shader_library.hpp>
#include <directx12-wrapper/extensions/dxc.hpp>

raytracing::renderers::depth_renderer::depth_renderer(const runtime_service& service) : renderer(service)
{
	mCommandAllocator = wrapper::directx12::command_allocator::create(service.render_device.device());
	mCommandList = wrapper::directx12::graphics_command_list::create(service.render_device.device(), mCommandAllocator);

	mCommandAllocator.reset();
	mCommandList.reset(mCommandAllocator);

	// create top acceleration 
	{
		std::vector<wrapper::directx12::raytracing_instance> instances;

		for (size_t index = 0; index < service.scene.entities.size(); index++)
		{
			const auto& entity = service.scene.entities[index];

			if (entity.mesh.has_value())
			{
				wrapper::directx12::raytracing_instance instance;
				
				instance.geometry = service.resource_system.resource<wrapper::directx12::raytracing_geometry>(entity.mesh->name);
				instance.identity = static_cast<uint32>(index);

				const auto matrix = transpose(entity.transform.matrix());

				for (int r = 0; r < 3; r++)
				{
					for (int c = 0; c < 4; c++)
					{
						instance.transform[r][c] = matrix[r][c];
					}
				}

				instances.emplace_back(instance);
			}
		}

		mAcceleration = wrapper::directx12::raytracing_acceleration::create(
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE,
			mCommandList, service.render_device.device(),
			instances);
	}

	// create ray tracing resource(buffer...)
	{
		mFrameDataCpuBuffer = wrapper::directx12::buffer::create(
			service.render_device.device(),
			wrapper::directx12::resource_info::upload(),
			std::max(sizeof(frame_data), static_cast<size_t>(256)));
	}

	// create shader libraries
	{
		wrapper::directx12::shader_code shader = wrapper::directx12::extensions::compile_from_file_using_dxc(
			L"./resources/shaders/renderers/depth_renderer/ray_generation.hlsl", L"", L"lib_6_6");

		mShaderLibrary = wrapper::directx12::shader_library::create(shader, { L"ray_generation" });
	}

	// create descriptor heap/table
	{
		mDescriptorTable
			.add_uav_range({ "global_render_target_hdr", "global_render_target_sdr" }, 0, 2);

		mDescriptorHeap = wrapper::directx12::descriptor_heap::create(service.render_device.device(), mDescriptorTable);

		const auto render_target_hdr = service.resource_system.resource<runtime::resources::gpu_texture>(
			"RenderSystem.RenderTarget.HDR");
		const auto render_target_sdr = service.resource_system.resource<runtime::resources::gpu_texture>(
			"RenderSystem.RenderTarget.SDR");

		service.render_device.device().create_unordered_access_view(
			wrapper::directx12::resource_view::read_write_texture2d(render_target_hdr.data.format()),
			mDescriptorHeap.cpu_handle(mDescriptorTable.index("global_render_target_hdr")),
			render_target_hdr.data);

		service.render_device.device().create_unordered_access_view(
			wrapper::directx12::resource_view::read_write_texture2d(render_target_sdr.data.format()),
			mDescriptorHeap.cpu_handle(mDescriptorTable.index("global_render_target_sdr")),
			render_target_sdr.data);
	}

	// create root signature
	{
		mRootSignatureInfo
			.add_constant_buffer_view("global_frame_data", 0, 0)
			.add_shader_resource_view("global_acceleration", 0, 1)
			.add_descriptor_table("global_descriptor_table", mDescriptorTable);

		mRootSignature = wrapper::directx12::root_signature::create(
			D3D12_ROOT_SIGNATURE_FLAG_NONE, 
			service.render_device.device(), mRootSignatureInfo);
	}

	// create shader associations and raytracing pipeline
	{
		wrapper::directx12::raytracing_shader_association association
		{
			std::nullopt,
			wrapper::directx12::raytracing_shader_config
			{
				8,
				wrapper::directx12::raytracing_shader_config::default_payload_size
			},
			L"ray_generation"
		};

		wrapper::directx12::raytracing_pipeline_info pipeline_info;

		pipeline_info
			.set_raytracing_shader_associations({ association })
			.set_shader_libraries({ mShaderLibrary })
			.set_ray_generation(L"ray_generation")
			.set_root_signature(mRootSignature);
		
		mPipeline = wrapper::directx12::raytracing_pipeline::create(service.render_device.device(), pipeline_info);
	}

	// create shader table
	{
		mShaderTable = wrapper::directx12::raytracing_shader_table::create(
			{}, {}, {}, L"ray_generation");

		for (const auto& [name, record] : mShaderTable.records())
		{
			std::memcpy(mShaderTable.data() + record.address,
				mPipeline.shader_identifier(name),
				D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
		}

		wrapper::directx12::buffer upload = wrapper::directx12::buffer::create(
			service.render_device.device(), 
			wrapper::directx12::resource_info::upload(),
			mShaderTable.size());

		upload.copy_from_cpu(mShaderTable.data(), mShaderTable.size());

		mShaderTableGpuBuffer = wrapper::directx12::buffer::create(
			service.render_device.device(),
			wrapper::directx12::resource_info::common(),
			mShaderTable.size());

		mShaderTableGpuBuffer.copy_from(mCommandList, upload);
		mShaderTableGpuBuffer.barrier(mCommandList, 
			D3D12_RESOURCE_STATE_COPY_DEST, 
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

		mCommandList.close();

		service.render_device.queue().execute({ mCommandList });
		service.render_device.wait();
	}
}

void raytracing::renderers::depth_renderer::update(const runtime_service& service, const runtime_frame& frame)
{
	const real size_x = static_cast<real>(service.scene.film.size_x);
	const real size_y = static_cast<real>(service.scene.film.size_y);

	mFrameData.raster_to_camera = scenes::compute_raster_to_camera(service.scene.camera.fov, size_x, size_y, 0.1f, 1000.0f);
	mFrameData.camera_to_world = transpose(service.scene.camera.transform.matrix());
	mFrameData.camera_position = scenes::transform_point(service.scene.camera.transform, vector3(0));
	mFrameData.sample_index = mSampleIndex++;
}

void raytracing::renderers::depth_renderer::render(const runtime_service& service, const runtime_frame& frame)
{
	// update frame data
	mFrameDataCpuBuffer.copy_from_cpu(&mFrameData, sizeof(mFrameData));

	mCommandAllocator.reset();
	mCommandList.reset(mCommandAllocator);

	mCommandList->SetPipelineState1(mPipeline.get());
	mCommandList->SetComputeRootSignature(mRootSignature.get());

	mCommandList.set_descriptor_heaps({ mDescriptorHeap.get() });
	mCommandList.set_compute_constant_buffer_view(mRootSignatureInfo.index("global_frame_data"), mFrameDataCpuBuffer);
	mCommandList.set_compute_shader_resource_view(mRootSignatureInfo.index("global_acceleration"), mAcceleration.acceleration());
	mCommandList.set_compute_descriptor_table(mRootSignatureInfo.index("global_descriptor_table"), mDescriptorHeap.gpu_handle());

	const auto render_target = service.resource_system.resource<runtime::resources::gpu_texture>("RenderSystem.RenderTarget.HDR");

	D3D12_DISPATCH_RAYS_DESC dispatch_desc = {};

	dispatch_desc.Width = render_target.info.size_x;
	dispatch_desc.Height = render_target.info.size_y;
	dispatch_desc.Depth = 1;

	const auto shader_table_address = mShaderTableGpuBuffer->GetGPUVirtualAddress();

	dispatch_desc.RayGenerationShaderRecord.StartAddress = shader_table_address + mShaderTable.ray_generation().address;
	dispatch_desc.RayGenerationShaderRecord.SizeInBytes = mShaderTable.ray_generation().size;

	mCommandList->DispatchRays(&dispatch_desc);
	mCommandList.close();

	service.render_device.queue().execute({ mCommandList });
}
