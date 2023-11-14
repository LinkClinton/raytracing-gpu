#include "shade_renderer.hpp"

#include "internal/material.hpp"

#include <directx12-wrapper/shaders/shader_library.hpp>
#include <directx12-wrapper/extensions/dxc.hpp>

#include <ranges>

raytracing::renderers::shade_renderer::shade_renderer(const runtime_service& service) : renderer(service)
{
	mCommandAllocator = wrapper::directx12::command_allocator::create(service.render_device.device());
	mCommandList = wrapper::directx12::graphics_command_list::create(service.render_device.device(), mCommandAllocator);

	mCommandAllocator.reset();
	mCommandList.reset(mCommandAllocator);

	std::vector<entity_data> entities_data;

	std::vector<raytracing::renderer::internal::diffuse_material> diffuse_materials;

	mapping<std::string, uint32> geometry_indexer;
	mapping<std::string, uint32> texture_indexer;

	// create render data
	{
		for (const auto& entity : service.scene.entities)
		{
			entity_data data;

			if (entity.mesh.has_value())
			{
				if (!geometry_indexer.contains(entity.mesh->name))
				{
					geometry_indexer.insert({ entity.mesh->name, static_cast<uint32>(geometry_indexer.size()) });
				}

				data.geometry_index = geometry_indexer.at(entity.mesh->name);
			}

			if (entity.material.has_value())
			{
				for (const auto& texture : entity.material->textures | std::views::values)
				{
					if (!texture.empty() && !texture_indexer.contains(texture))
					{
						texture_indexer.insert({ texture, static_cast<uint32>(texture_indexer.size()) });
					}
				}

				if (entity.material->type == "diffuse")
				{
					raytracing::renderer::internal::diffuse_material material;

					material.diffuse = raytracing::renderer::internal::read_property_from_material(
						texture_indexer,
						entity.material.value(), 
						"diffuse");

					data.material_index = static_cast<uint32>(diffuse_materials.size());
					data.material_type = static_cast<uint32>(raytracing::renderer::internal::material_type::diffuse);

					diffuse_materials.push_back(material);
				}
			}
			
			entities_data.push_back(data);
		}
	}

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

		mEntitiesDataCpuBuffer = wrapper::directx12::buffer::create(
			service.render_device.device(),
			wrapper::directx12::resource_info::upload(),
			entities_data.size() * sizeof(entity_data));

		mEntitiesDataGpuBuffer = wrapper::directx12::buffer::create(
			service.render_device.device(),
			wrapper::directx12::resource_info::common(),
			entities_data.size() * sizeof(entity_data));

		mEntitiesDataCpuBuffer.copy_from_cpu(entities_data.data(), entities_data.size() * sizeof(entity_data));
		mEntitiesDataGpuBuffer.copy_from(mCommandList, mEntitiesDataCpuBuffer);
		mEntitiesDataGpuBuffer.barrier(mCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}

	// create shader libraries
	{
		wrapper::directx12::shader_code shader = wrapper::directx12::extensions::compile_from_file_using_dxc(
			L"./resources/shaders/renderers/shade_renderer/ray_generation.hlsl", L"", L"lib_6_6");

		mShaderLibrary = wrapper::directx12::shader_library::create(shader, { L"ray_generation" });
	}

	// create descriptor heap/table
	{
		std::vector<std::string> global_geometry_positions_name;
		std::vector<std::string> global_geometry_normals_name;
		std::vector<std::string> global_geometry_uvs_name;
		std::vector<std::string> global_geometry_indices_name;

		for (size_t index = 0; index < geometry_indexer.size(); index++)
		{
			global_geometry_positions_name.push_back("global_geometry_positions_" + std::to_string(index));
			global_geometry_normals_name.push_back("global_geometry_normals_" + std::to_string(index));
			global_geometry_uvs_name.push_back("global_geometry_uvs_" + std::to_string(index));
			global_geometry_indices_name.push_back("global_geometry_indices_" + std::to_string(index));
		}

		std::vector<std::string> global_texture_name;

		for (size_t index = 0; index < texture_indexer.size(); index++)
		{
			global_texture_name.push_back("global_texture_" + std::to_string(index));
		}

		mDescriptorTable
			.add_uav_range({ "global_render_target_hdr", "global_render_target_sdr" }, 0, 2)
			.add_srv_range(global_geometry_positions_name, 0, 3)
			.add_srv_range(global_geometry_normals_name, 0, 4)
			.add_srv_range(global_geometry_uvs_name, 0, 5)
			.add_srv_range(global_geometry_indices_name, 0, 6)
			.add_srv_range(global_texture_name, 0, 7);

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

		// create geometry(positions, normals, uvs, indices) gpu buffer view
		for (const auto& [name, index] : geometry_indexer)
		{
			const auto& [info, data] = service.resource_system.resource<runtime::resources::gpu_mesh>(name);

			service.render_device.device().create_shader_resource_view(
				wrapper::directx12::resource_view::structured_buffer(sizeof(vector3), data.positions.size_in_bytes()),
				mDescriptorHeap.cpu_handle(mDescriptorTable.index(global_geometry_positions_name[index])),
				data.positions);

			service.render_device.device().create_shader_resource_view(
				wrapper::directx12::resource_view::structured_buffer(sizeof(vector3), data.normals.size_in_bytes()),
				mDescriptorHeap.cpu_handle(mDescriptorTable.index(global_geometry_normals_name[index])),
				data.normals);

			service.render_device.device().create_shader_resource_view(
				wrapper::directx12::resource_view::structured_buffer(sizeof(vector3), data.uvs.size_in_bytes()),
				mDescriptorHeap.cpu_handle(mDescriptorTable.index(global_geometry_uvs_name[index])),
				data.uvs);

			service.render_device.device().create_shader_resource_view(
				wrapper::directx12::resource_view::structured_buffer(sizeof(uint32) * 3, data.indices.size_in_bytes()),
				mDescriptorHeap.cpu_handle(mDescriptorTable.index(global_geometry_indices_name[index])),
				data.indices);
		}

		// create texture view
		for (const auto& [name, index] : texture_indexer)
		{
			const auto& [info, data] = service.resource_system.resource<runtime::resources::gpu_texture>(name);

			service.render_device.device().create_shader_resource_view(
				wrapper::directx12::resource_view::texture2d(data.format()),
				mDescriptorHeap.cpu_handle(mDescriptorTable.index(global_texture_name[index])),
				data);
		}
	}

	// create root signature
	{
		mRootSignatureInfo
			.add_constant_buffer_view("global_frame_data", 0, 0)
			.add_shader_resource_view("global_entities_data", 1, 0)
			.add_shader_resource_view("global_acceleration", 0, 1)
			.add_descriptor_table("global_descriptor_table", mDescriptorTable);

		mRootSignature = wrapper::directx12::root_signature::create(service.render_device.device(), mRootSignatureInfo);
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

		mShaderTableCpuBuffer = wrapper::directx12::buffer::create(
			service.render_device.device(),
			wrapper::directx12::resource_info::upload(),
			mShaderTable.size());

		mShaderTableCpuBuffer.copy_from_cpu(mShaderTable.data(), mShaderTable.size());

		mShaderTableGpuBuffer = wrapper::directx12::buffer::create(
			service.render_device.device(),
			wrapper::directx12::resource_info::common(),
			mShaderTable.size());

		mShaderTableGpuBuffer.copy_from(mCommandList, mShaderTableCpuBuffer);
		mShaderTableGpuBuffer.barrier(mCommandList,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
	}

	mCommandList.close();

	service.render_device.queue().execute({ mCommandList });
	service.render_device.wait();
}

void raytracing::renderers::shade_renderer::update(const runtime_service& service, const runtime_frame& frame)
{
	const real size_x = static_cast<real>(service.scene.film.size_x);
	const real size_y = static_cast<real>(service.scene.film.size_y);

	mFrameData.raster_to_camera = scenes::compute_raster_to_camera(service.scene.camera.fov, size_x, size_y, 0.1f, 1000.0f);
	mFrameData.camera_to_world = transpose(service.scene.camera.transform.matrix());
	mFrameData.camera_position = scenes::transform_point(service.scene.camera.transform, vector3(0));
	mFrameData.sample_index = mSampleIndex++;
}

void raytracing::renderers::shade_renderer::render(const runtime_service& service, const runtime_frame& frame)
{
	// update frame data
	mFrameDataCpuBuffer.copy_from_cpu(&mFrameData, sizeof(mFrameData));

	mCommandAllocator.reset();
	mCommandList.reset(mCommandAllocator);

	mCommandList->SetPipelineState1(mPipeline.get());
	mCommandList->SetComputeRootSignature(mRootSignature.get());

	mCommandList.set_descriptor_heaps({ mDescriptorHeap.get() });
	mCommandList.set_compute_constant_buffer_view(mRootSignatureInfo.index("global_frame_data"), mFrameDataCpuBuffer);
	mCommandList.set_compute_shader_resource_view(mRootSignatureInfo.index("global_entities_data"), mEntitiesDataGpuBuffer);
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
