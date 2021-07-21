#include "depth_renderer.hpp"

#include <directx12-wrapper/extensions/dxc.hpp>

namespace path_tracing::renderers {

	const wchar_t* depth_renderer_ray_generation = L"ray_generation";
	const wchar_t* depth_renderer_closest_hit = L"closest_hit";
	const wchar_t* depth_renderer_miss_shader = L"miss_shader";
	
}

path_tracing::renderers::depth_renderer::depth_renderer(const runtime_service& service) : renderer(service)
{
	mCommandAllocator = wrapper::directx12::command_allocator::create(service.render_device.device());
	mCommandList = wrapper::directx12::graphics_command_list::create(service.render_device.device(), mCommandAllocator);

	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.get(), nullptr);

	build_acceleration(service);

	build_shader_libraries(service);

	build_hit_groups(service);

	build_descriptor_heap(service);
	
	build_root_signature(service);

	build_shader_association(service);

	build_raytracing_pipeline(service);

	build_raytracing_shader_table(service);
	
	mCommandList.close();
	
	service.render_device.queue().execute({ mCommandList });
	service.render_device.wait();
}

void path_tracing::renderers::depth_renderer::update(const runtime_service& service, const runtime_frame& frame)
{
	const auto [raster_to_camera, camera_to_world] = compute_camera_matrix(
		service.scene);

	if (mSceneConfig.raster_to_camera != raster_to_camera || mSceneConfig.camera_to_world != camera_to_world)
		mSampleIndex = 0;
	
	mSceneConfig.raster_to_camera = raster_to_camera;
	mSceneConfig.camera_to_world = camera_to_world;
	mSceneConfig.camera_position = transform_point(service.scene.camera.transform, vector3(0));
	mSceneConfig.sample_index = mSampleIndex++;
}

void path_tracing::renderers::depth_renderer::render(const runtime_service& service, const runtime_frame& frame)
{
	const auto render_target = service.resource_system.resource<wrapper::directx12::texture2d>(
		"RenderSystem.RenderTarget.HDR");

	mSceneConfigCpuBuffer.copy_from_cpu(&mSceneConfig, sizeof(depth_renderer_scene_config));

	const auto table_address = mRaytracingShaderTableGpuBuffer->GetGPUVirtualAddress();
	
	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.get(), nullptr);

	mCommandList->SetPipelineState1(mRaytracingPipeline.get());
	mCommandList->SetComputeRootSignature(mRootSignature[0].get());
	
	mCommandList.set_descriptor_heaps({ mDescriptorHeap.get() });
	mCommandList.set_compute_descriptor_table(mRootSignatureInfo->index("descriptor_table"), mDescriptorHeap.gpu_handle());

	D3D12_DISPATCH_RAYS_DESC desc = {};

	desc.Width = static_cast<UINT>(render_target.size_x());
	desc.Height = static_cast<UINT>(render_target.size_y());
	desc.Depth = 1;

	desc.RayGenerationShaderRecord.StartAddress = table_address + mRaytracingShaderTable.ray_generation().address;
	desc.RayGenerationShaderRecord.SizeInBytes = mRaytracingShaderTable.ray_generation().size;
	desc.MissShaderTable.StartAddress = table_address + mRaytracingShaderTable.miss_shaders().address;
	desc.MissShaderTable.StrideInBytes = mRaytracingShaderTable.miss_shaders().size;
	desc.MissShaderTable.SizeInBytes = mRaytracingShaderTable.miss_shaders().size * mRaytracingPipelineInfo.miss_shaders().size();
	desc.HitGroupTable.StartAddress = table_address + mRaytracingShaderTable.hit_groups().address;
	desc.HitGroupTable.StrideInBytes = mRaytracingShaderTable.hit_groups().size;
	desc.HitGroupTable.SizeInBytes = mRaytracingShaderTable.hit_groups().size * mRaytracingPipelineInfo.hit_groups().size();

	mCommandList->DispatchRays(&desc);
	
	mCommandList.close();

	service.render_device.queue().execute({ mCommandList });
}

void path_tracing::renderers::depth_renderer::build_acceleration(const runtime_service& service)
{
	std::vector<wrapper::directx12::raytracing_instance> instances;

	for (const auto& entity : service.scene.entities) {
		wrapper::directx12::raytracing_instance instance;

		instance.geometry = service.meshes_system.geometry(entity.mesh.value());

		const auto matrix = glm::transpose(entity.transform.matrix());

		for (int r = 0; r < 3; r++) for (int c = 0; c < 4; c++) instance.transform[r][c] = matrix[r][c];

		instances.push_back(instance);
	}

	mAcceleration = wrapper::directx12::raytracing_acceleration::create(
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE,
		mCommandList, service.render_device.device(),
		instances
	);
}

void path_tracing::renderers::depth_renderer::build_shader_libraries(const runtime_service& service)
{
	std::vector<wrapper::directx12::shader_library> libraries = {
		wrapper::directx12::shader_library::create(
			wrapper::directx12::extensions::compile_from_file_using_dxc(
			L"./resources/shaders/depth_renderer/ray_generation.hlsl", L"", L"lib_6_4"),
			{ depth_renderer_ray_generation }
		),
		wrapper::directx12::shader_library::create(
			wrapper::directx12::extensions::compile_from_file_using_dxc(
			L"./resources/shaders/depth_renderer/closest_hits.hlsl", L"", L"lib_6_4"),
			{ depth_renderer_closest_hit }
		),
		wrapper::directx12::shader_library::create(
			wrapper::directx12::extensions::compile_from_file_using_dxc(
			L"./resources/shaders/depth_renderer/miss_shaders.hlsl", L"", L"lib_6_4"),
			{ depth_renderer_miss_shader }
		)
	};

	mRaytracingPipelineInfo
		.set_shader_libraries(libraries)
		.set_ray_generation(depth_renderer_ray_generation)
		.set_miss_shaders({ depth_renderer_miss_shader });
}

void path_tracing::renderers::depth_renderer::build_hit_groups(const runtime_service& service)
{
	std::vector<wrapper::directx12::raytracing_hit_group> hit_groups;
	
	for (size_t index = 0; index < service.scene.entities.size(); index++) {
		hit_groups.push_back({
			D3D12_HIT_GROUP_TYPE_TRIANGLES,
			L"", depth_renderer_closest_hit, L"",
			L"hit_group_" + std::to_wstring(index)
		});
	}

	mRaytracingPipelineInfo.set_raytracing_hit_groups(hit_groups);
}

void path_tracing::renderers::depth_renderer::build_descriptor_heap(const runtime_service& service)
{
	mSceneConfigCpuBuffer = wrapper::directx12::buffer::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::upload(),
		std::max(sizeof(depth_renderer_scene_config), static_cast<size_t>(256)));
	
	mDescriptorTable
		.add_cbv_range({ "scene_config" }, 0, 0)
		.add_srv_range({ "acceleration" }, 0, 1)
		.add_uav_range({ "render_target_hdr", "render_target_sdr" }, 0, 2);
	
	mDescriptorHeap = wrapper::directx12::descriptor_heap::create(service.render_device.device(), mDescriptorTable);

	const auto render_target_hdr = service.resource_system.resource<wrapper::directx12::texture2d>(
		"RenderSystem.RenderTarget.HDR");
	const auto render_target_sdr = service.resource_system.resource<wrapper::directx12::texture2d>(
		"RenderSystem.RenderTarget.SDR");
	
	service.render_device.device().create_constant_buffer_view(
		wrapper::directx12::resource_view::constant_buffer(mSceneConfigCpuBuffer),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("scene_config"))
	);

	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::acceleration(mAcceleration),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("acceleration"))
	);

	service.render_device.device().create_unordered_access_view(
		wrapper::directx12::resource_view::read_write_texture2d(render_target_hdr.format()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("render_target_hdr")),
		render_target_hdr);

	service.render_device.device().create_unordered_access_view(
		wrapper::directx12::resource_view::read_write_texture2d(render_target_sdr.format()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("render_target_sdr")),
		render_target_sdr);
}

void path_tracing::renderers::depth_renderer::build_root_signature(const runtime_service& service)
{
	mRootSignatureInfo[0]
		.add_descriptor_table("descriptor_table", mDescriptorTable);

	mRootSignature[0] = wrapper::directx12::root_signature::create(service.render_device.device(), mRootSignatureInfo[0], false);
	mRootSignature[1] = wrapper::directx12::root_signature::create(service.render_device.device(), mRootSignatureInfo[1], true);

	mRaytracingPipelineInfo.set_root_signature(mRootSignature[0]);
}

void path_tracing::renderers::depth_renderer::build_shader_association(const runtime_service& service)
{
	const wrapper::directx12::raytracing_shader_config config = {
		8,
		wrapper::directx12::raytracing_shader_config::default_payload_size
	};
	
	std::vector<wrapper::directx12::raytracing_shader_association> associations;

	// association of shaders(ray_generation, miss_shaders, closest_hit_shader)
	associations.push_back({ std::nullopt, config, depth_renderer_ray_generation });
	associations.push_back({ std::nullopt, config, depth_renderer_closest_hit });
	associations.push_back({ std::nullopt, config, depth_renderer_miss_shader });

	for (const auto& hit_group : mRaytracingPipelineInfo.hit_groups()) {
		associations.push_back({
			wrapper::directx12::raytracing_shader_root_signature{
				mRootSignature[1], mRootSignatureInfo[1].size()
			}, std::nullopt, hit_group.name
		});
	}

	mRaytracingPipelineInfo.set_raytracing_shader_associations(associations);
}

void path_tracing::renderers::depth_renderer::build_raytracing_pipeline(const runtime_service& service)
{
	mRaytracingPipelineInfo.set_max_depth(5);
	
	mRaytracingPipeline = wrapper::directx12::raytracing_pipeline::create(service.render_device.device(), mRaytracingPipelineInfo);
}

void path_tracing::renderers::depth_renderer::build_raytracing_shader_table(const runtime_service& service)
{
	std::vector<std::wstring> names;

	for (const auto& hit_group : mRaytracingPipelineInfo.hit_groups())
		names.push_back(hit_group.name);
	
	mRaytracingShaderTable = wrapper::directx12::raytracing_shader_table::create(
		mRaytracingPipelineInfo.associations(),
		{ depth_renderer_miss_shader },
		names, depth_renderer_ray_generation
	);

	for (const auto& record : mRaytracingShaderTable.records()) {
		std::memcpy(mRaytracingShaderTable.data() + record.second.address,
			mRaytracingPipeline.shader_identifier(record.first),
			D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	}

	mRaytracingShaderTableCpuBuffer = wrapper::directx12::buffer::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::upload(),
		mRaytracingShaderTable.size()
	);

	mRaytracingShaderTableGpuBuffer = wrapper::directx12::buffer::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_COPY_DEST),
		mRaytracingShaderTable.size()
	);

	mRaytracingShaderTableCpuBuffer.copy_from_cpu(mRaytracingShaderTable.data(), mRaytracingShaderTable.size());
	mRaytracingShaderTableGpuBuffer.copy_from(mCommandList, mRaytracingShaderTableCpuBuffer);
	mRaytracingShaderTableGpuBuffer.barrier(mCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
}
