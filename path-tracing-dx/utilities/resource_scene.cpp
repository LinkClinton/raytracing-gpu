#include "resource_scene.hpp"

path_tracing::dx::utilities::resource_scene::resource_scene(const std::shared_ptr<device>& device) :
	mDevice(device)
{
	mCommandAllocator = std::make_shared<command_allocator>(mDevice);
	mCommandList = std::make_shared<graphics_command_list>(mCommandAllocator, mDevice);

	mDescriptorTable = std::make_shared<descriptor_table>();

	mDescriptorTable->add_cbv_range({ "scene_info" }, 0, 0);
	mDescriptorTable->add_srv_range({ "acceleration", "materials", "emitters" }, 0, 1);
	mDescriptorTable->add_uav_range({ "render_target" }, 0, 2);

	mDescriptorHeap = std::make_shared<descriptor_heap>(mDevice, mDescriptorTable);

	mRootSignature = std::make_shared<root_signature>();
	mRootSignature->add_descriptor_table("global_heap", mDescriptorTable);
	mRootSignature->serialize(mDevice);

	mSceneInfo = std::make_shared<buffer>(mDevice, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_NONE,
		D3D12_HEAP_TYPE_UPLOAD, sizeof(scene_info));
}

void path_tracing::dx::utilities::resource_scene::set_render_target(const std::shared_ptr<texture2d>& render_target)
{
	mRenderTarget = render_target;
}

void path_tracing::dx::utilities::resource_scene::set_entities(
	const std::vector<std::shared_ptr<entity>>& entities, const std::shared_ptr<resource_cache>& cache)
{
	for (const auto& entity : entities) {
		const auto cache_data = cache->cache(entity);

		if (cache_data.material.has_value()) mMaterialsData.push_back(cache_data.material.value());
		if (cache_data.emitter.has_value()) mEmittersData.push_back(cache_data.emitter.value());
		if (cache_data.instance.has_value()) mInstancesData.push_back(cache_data.instance.value());
	}
}

void path_tracing::dx::utilities::resource_scene::set_scene_info(const scene_info& info)
{
	mSceneInfoData = info;
	
	std::memcpy(mSceneInfo->map(), &mSceneInfoData, sizeof(scene_info)); mSceneInfo->unmap();
}

void path_tracing::dx::utilities::resource_scene::execute(const std::shared_ptr<command_queue>& queue)
{
	(*mCommandAllocator)->Reset();
	(*mCommandList)->Reset(mCommandAllocator->get(), nullptr);
	
	mAcceleration = std::make_shared<raytracing_acceleration>(mInstancesData);

	mAcceleration->build(D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE,
		mCommandList, mDevice);

	std::shared_ptr<buffer> cpu_materials_buffer;
	std::shared_ptr<buffer> cpu_emitters_buffer;

	copy_data_to_gpu_buffer(mCommandList, mDevice, mMaterialsData, cpu_materials_buffer, mMaterials);
	copy_data_to_gpu_buffer(mCommandList, mDevice, mEmittersData, cpu_emitters_buffer, mEmitters);

	(*mCommandList)->Close();
	
	queue->execute({ mCommandList });
	
	D3D12_CONSTANT_BUFFER_VIEW_DESC scene_info_desc = {};

	scene_info_desc.BufferLocation = (*mSceneInfo)->GetGPUVirtualAddress();
	scene_info_desc.SizeInBytes = static_cast<UINT>(mSceneInfo->size());

	D3D12_SHADER_RESOURCE_VIEW_DESC acceleration_desc = {};

	acceleration_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	acceleration_desc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	acceleration_desc.Format = DXGI_FORMAT_UNKNOWN;
	acceleration_desc.RaytracingAccelerationStructure.Location = (*mAcceleration->data())->GetGPUVirtualAddress();

	D3D12_SHADER_RESOURCE_VIEW_DESC materials_desc = {};

	materials_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	materials_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	materials_desc.Format = DXGI_FORMAT_UNKNOWN;
	materials_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	materials_desc.Buffer.FirstElement = 0;
	materials_desc.Buffer.NumElements = static_cast<UINT>(mMaterialsData.size());
	materials_desc.Buffer.StructureByteStride = sizeof(material_gpu_buffer);

	D3D12_SHADER_RESOURCE_VIEW_DESC emitters_desc = {};

	emitters_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	emitters_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	emitters_desc.Format = DXGI_FORMAT_UNKNOWN;
	emitters_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	emitters_desc.Buffer.FirstElement = 0;
	emitters_desc.Buffer.NumElements = static_cast<UINT>(mEmittersData.size());
	emitters_desc.Buffer.StructureByteStride = sizeof(emitter_gpu_buffer);

	D3D12_UNORDERED_ACCESS_VIEW_DESC render_target_desc = {};

	render_target_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	render_target_desc.Format = mRenderTarget->format();
	render_target_desc.Texture2D.PlaneSlice = 0;
	render_target_desc.Texture2D.MipSlice = 0;

	const auto scene_info_index = mDescriptorTable->index("scene_info");
	const auto acceleration_index = mDescriptorTable->index("acceleration");
	const auto materials_index = mDescriptorTable->index("materials");
	const auto emitters_index = mDescriptorTable->index("emitters");
	const auto render_target_index = mDescriptorTable->index("render_target");

	(*mDevice)->CreateConstantBufferView(&scene_info_desc, mDescriptorHeap->cpu_handle(scene_info_index));
	(*mDevice)->CreateShaderResourceView(nullptr, &acceleration_desc, mDescriptorHeap->cpu_handle(acceleration_index));
	(*mDevice)->CreateShaderResourceView(mMaterials->get(), &materials_desc, mDescriptorHeap->cpu_handle(materials_index));
	(*mDevice)->CreateShaderResourceView(mEmitters->get(), &materials_desc, mDescriptorHeap->cpu_handle(emitters_index));
	(*mDevice)->CreateUnorderedAccessView(mRenderTarget->get(), nullptr, 
		&render_target_desc, mDescriptorHeap->cpu_handle(render_target_index));

	queue->wait();
}

void path_tracing::dx::utilities::resource_scene::render(const std::shared_ptr<graphics_command_list>& command_list) const
{
	(*command_list)->SetDescriptorHeaps(1, { mDescriptorHeap->get_address_of() });
	(*command_list)->SetComputeRootSignature(mRootSignature->get());
	(*command_list)->SetComputeRootDescriptorTable(0, mDescriptorHeap->gpu_handle());
}

std::shared_ptr<path_tracing::dx::wrapper::texture2d> path_tracing::dx::utilities::resource_scene::render_target() const noexcept
{
	return mRenderTarget;
}

std::shared_ptr<path_tracing::dx::wrapper::root_signature> path_tracing::dx::utilities::resource_scene::signature() const noexcept
{
	return mRootSignature;
}
