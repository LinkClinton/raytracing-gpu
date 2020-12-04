#include "module_renderer.hpp"

#include "directx12-wrapper/extensions/dxc.hpp"

namespace path_tracing::renderers {

	using scenes::components::submodule_data;
	using scenes::components::transform;
	using runtime::resources::mesh_info;
	
	const wchar_t* module_renderer_ray_generation = L"ray_generation";
	const wchar_t* module_renderer_closest_hit = L"closest_hit";
	const wchar_t* module_renderer_miss_shader = L"miss_shader";

	template <typename T>
	void update_submodule_data_element(mapping<std::string, std::string>& elements, 
		const mapping<std::string, T>& values, const std::string& type)
	{
		for (const auto& value : values) {
			if (elements.find(value.first) == elements.end())
				elements.insert({ value.first, type });
			else {
				assert(elements[value.first] == type);
			}
		}
	}
	
	void update_submodule_data_element(mapping<std::string, std::string>& elements, const submodule_data& data)
	{
		update_submodule_data_element(elements, data.float3, "float3");
		update_submodule_data_element(elements, data.real, "real");
		update_submodule_data_element(elements, data.uint, "uint");

		// we do not support texture in current version
		/*for (const auto& value : data.texture) {
			assert(elements.find(value.first) != elements.end());

			if (elements.find(value.first + "_index") == elements.end())
				elements.insert({ value.first + "_index", "uint" });
		}*/
	}

	auto packing_submodule_data_layout(const mapping<std::string, std::string>& elements) -> std::vector<wrapper::directx12::shader_variable>
	{
		/*
		 * packing rule :
		 * 1. we push 12bytes elements(float3) with 4bytes element(real, uint)
		 * 2. we push 4bytes elements(real, uint) and add unused element
		 */
		std::vector<std::pair<std::string, std::string>> element_in_12bytes;
		std::vector<std::pair<std::string, std::string>> element_in_04bytes;

		std::vector<wrapper::directx12::shader_variable> packing_layout;
		
		for (const auto& element : elements) {
			if (element.second == "real" || element.second == "uint") element_in_04bytes.push_back(element);
			if (element.second == "float3") element_in_12bytes.push_back(element);
		}

		uint32 unused_index = 0;
		
		while (element_in_12bytes.size() > element_in_04bytes.size())
			element_in_04bytes.push_back({ "unused" + std::to_string(unused_index++), "real" });

		while ((element_in_04bytes.size() - element_in_12bytes.size()) % 4 != 0)
			element_in_04bytes.push_back({ "unused" + std::to_string(unused_index++), "real" });

		for (size_t index = 0; index < element_in_12bytes.size(); index++) {
			packing_layout.push_back({ "", element_in_12bytes[index].first, element_in_12bytes[index].second });
			packing_layout.push_back({ "", element_in_04bytes[index].first, element_in_04bytes[index].second });
		}

		for (size_t index = element_in_12bytes.size(); index < element_in_04bytes.size(); index++)
			packing_layout.push_back({ "", element_in_04bytes[index].first, element_in_04bytes[index].second });

		return packing_layout;
	}

	module_renderer_mesh_info allocate_mesh_info(const runtime_service& service, const transform& transform, const mesh_info& info)
	{
		const auto property = service.meshes_system.property(info);
		
		return {
			info.idx_count / 3, info.vtx_location, info.idx_location / 3,
			0, property.normals ? 1u : 0u, property.uvs ? 1u : 0u,
			scenes::compute_mesh_area_with_transform(service.meshes_system, transform, info)
		};
	}

	template <typename T>
	void copy_submodule_data_to(byte* target, const mapping<std::string, T>& elements, const mapping<std::string, uint32>& addresses)
	{
		for (const auto& element : elements) 
			std::memcpy(target + addresses.at(element.first), &element.second, sizeof(T));
	}
	
	void copy_submodule_data_to(byte* target, const submodule_data& data, const mapping<std::string, uint32>& addresses)
	{
		copy_submodule_data_to(target, data.float3, addresses);
		copy_submodule_data_to(target, data.uint, addresses);
		copy_submodule_data_to(target, data.real, addresses);
	}
}

path_tracing::renderers::module_renderer::module_renderer(
	const std::vector<submodule>& materials, const std::vector<submodule>& lights, 
	const runtime_service& service) : renderer(service)
{
	mCommandAllocator = wrapper::directx12::command_allocator::create(service.render_device.device());
	mCommandList = wrapper::directx12::graphics_command_list::create(service.render_device.device(), mCommandAllocator);

	mCommandAllocator->Reset();
	mCommandList->Reset(mCommandAllocator.get(), nullptr);

	build_acceleration(service);
	build_material_submodule(service, materials);
	build_light_submodule(service, lights);
	build_macro_submodule(service);
	build_shader_libraries(service);
	build_hit_groups(service);
	build_structured_buffer(service);
	build_descriptor_heap(service);
	build_root_signature(service);
	build_shader_association(service);
	build_raytracing_pipeline(service);
	build_raytracing_shader_table(service);
	
	mCommandList.close();

	service.render_device.queue().execute({ mCommandList });
	service.render_device.wait();
}

void path_tracing::renderers::module_renderer::update(const runtime_service& service, const runtime_frame& frame)
{
	const auto [raster_to_camera, camera_to_world] = compute_camera_matrix(
		service.scene);

	mSceneConfig.raster_to_camera = raster_to_camera;
	mSceneConfig.camera_to_world = camera_to_world;
	mSceneConfig.camera_position = transform_point(service.scene.camera.transform, vector3(0));
	mSceneConfig.sample_index = static_cast<uint32>(frame.frame_index);
	mSceneConfig.max_depth = service.scene.max_depth;
}

void path_tracing::renderers::module_renderer::render(const runtime_service& service, const runtime_frame& frame)
{
	const auto render_target = service.resource_system.resource<wrapper::directx12::texture2d>(
		"RenderSystem.RenderTarget.HDR");

	mSceneConfigCpuBuffer.copy_from_cpu(&mSceneConfig, sizeof(module_renderer_entity_info));

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

void path_tracing::renderers::module_renderer::build_material_submodule(const runtime_service& service,
	const std::vector<submodule>& materials)
{
	mapping<std::string, std::string> submodule_data_elements;

	for (const auto& entity : service.scene.entities) {
		if (!entity.material.has_value()) continue;

		const auto& material = entity.material.value();

		if (mMaterialTypes.find(material.submodule) == mMaterialTypes.end())
			mMaterialTypes.insert({ material.submodule, static_cast<uint32>(mMaterialTypes.size()) });

		update_submodule_data_element(submodule_data_elements, material);
	}

	submodule_data_elements.insert({ "type", "uint" });

	const auto packed_material_layout = packing_submodule_data_layout(submodule_data_elements);
	const auto common_material_layout = packing_submodule_data_layout(submodule_data_elements);

	mMaterialSubmoduleTypeSize = 0;
	
	for (const auto& variable : packed_material_layout) {
		mMaterialValuesMemoryAddress.insert({ variable.name, static_cast<uint32>(mMaterialSubmoduleTypeSize) });

		mMaterialSubmoduleTypeSize += scenes::components::size_of_submodule_data_type(variable.type);
	}
	
	mMaterialSubmodule = wrapper::directx12::shader_creator::create_from_file(
		L"./resources/shaders/module_renderer/templates/material.hlsl");

	auto packed_material_struct = wrapper::directx12::shader_creator::create();
	auto common_material_struct = wrapper::directx12::shader_creator::create();

	for (const auto& variable : packed_material_layout)
		packed_material_struct.define_variable(variable.type, variable.name, 1);

	for (const auto& variable : common_material_layout)
		common_material_struct.define_variable(variable.type, variable.name, 1);

	auto include_material_submodule = wrapper::directx12::shader_creator::create();

	for (const auto& submodule : materials) {
		if (mMaterialTypes.find(submodule.name) == mMaterialTypes.end()) continue;

		include_material_submodule.include(submodule.file);
	}

	auto material_functions_sentences = mapping<std::string, wrapper::directx12::shader_creator>{
		{ "unpacking", wrapper::directx12::shader_creator::create() },
		{ "evaluate", wrapper::directx12::shader_creator::create() },
		{ "sample", wrapper::directx12::shader_creator::create() },
		{ "pdf", wrapper::directx12::shader_creator::create() }
	};

	// todo : unpacking material with texture 
	for (const auto& variable : common_material_layout)
		material_functions_sentences["unpacking"].add_sentence(
			"material." + variable.name + " = packed_material." + variable.name + ";", 1);

	for (const auto& material : mMaterialTypes) {
		material_functions_sentences["evaluate"].add_sentence("case " + std::to_string(material.second) + ": return evaluate_" + 
			material.first + "(material, wo, wi);", 1);
	}

	for (const auto& material : mMaterialTypes) {
		material_functions_sentences["sample"].add_sentence("case " + std::to_string(material.second) + ": return sample_" +
			material.first + "(material, wo, value);", 1);
	}

	for (const auto& material : mMaterialTypes) {
		material_functions_sentences["pdf"].add_sentence("case " + std::to_string(material.second) + ": return pdf_" +
			material.first + "(material, wo, wi);", 1);
	}
	
	mMaterialSubmodule.replace("[packed_material]", packed_material_struct);
	mMaterialSubmodule.replace("[common_material]", common_material_struct);
	
	mMaterialSubmodule.replace("[include_material_submodule]", include_material_submodule);

	for (const auto& function_sentences : material_functions_sentences) 
		mMaterialSubmodule.replace("[" + function_sentences.first + "_material]", function_sentences.second);
}

void path_tracing::renderers::module_renderer::build_light_submodule(const runtime_service& service,
	const std::vector<submodule>& lights)
{
	mapping<std::string, std::string> submodule_data_elements;

	for (const auto& entity : service.scene.entities) {
		if (!entity.light.has_value()) continue;

		const auto light = entity.light.value();
		
		if (mLightTypes.find(light.submodule) == mLightTypes.end())
			mLightTypes.insert({ light.submodule, static_cast<uint32>(mLightTypes.size()) });

		update_submodule_data_element(submodule_data_elements, light);
	}

	submodule_data_elements.insert({ "type", "uint" });

	const auto common_light_layout = packing_submodule_data_layout(submodule_data_elements);

	mLightSubmoduleTypeSize = 0;

	for (const auto& variable : common_light_layout) {
		mLightValuesMemoryAddress.insert({ variable.name, static_cast<uint32>(mLightSubmoduleTypeSize) });

		mLightSubmoduleTypeSize += scenes::components::size_of_submodule_data_type(variable.type);
	}
	
	mLightSubmodule = wrapper::directx12::shader_creator::create_from_file(
		L"./resources/shaders/module_renderer/templates/light.hlsl");

	auto common_light_struct = wrapper::directx12::shader_creator::create();

	for (const auto& variable : common_light_layout)
		common_light_struct.define_variable(variable.type, variable.name, 1);

	auto include_light_submodule = wrapper::directx12::shader_creator::create();

	for (const auto& submodule : lights) {
		if (mLightTypes.find(submodule.name) == mLightTypes.end()) continue;

		include_light_submodule.include(submodule.file);
	}

	auto light_functions_sentences = mapping<std::string, wrapper::directx12::shader_creator>{
		{ "evaluate", wrapper::directx12::shader_creator::create() },
		{ "sample", wrapper::directx12::shader_creator::create() },
		{ "pdf", wrapper::directx12::shader_creator::create() }
	};

	for (const auto& light : mLightTypes) {
		light_functions_sentences["evaluate"].add_sentence("case " + std::to_string(light.second) + ": return evaluate_" +
			light.first + "(light, interaction, wi);", 1);
	}

	for (const auto& light : mLightTypes) {
		light_functions_sentences["sample"].add_sentence("case " + std::to_string(light.second) + ": return sample_" +
			light.first + "(light, reference, value);", 1);
	}

	for (const auto& light : mLightTypes) {
		light_functions_sentences["pdf"].add_sentence("case " + std::to_string(light.second) + ": return pdf_" +
			light.first + "(light, reference, wi);", 1);
	}
	
	mLightSubmodule.replace("[light]", common_light_struct);

	mLightSubmodule.replace("[include_light_submodule]", include_light_submodule);

	for (const auto& function_sentences : light_functions_sentences)
		mLightSubmodule.replace("[" + function_sentences.first + "_light]", function_sentences.second);
}

void path_tracing::renderers::module_renderer::build_macro_submodule(const runtime_service& service)
{
	if (mLightTypes.find("environment_light") != mLightTypes.end())
		mMacroSubmodule.add_sentence("#define __ENABLE_ENVIRONMENT_LIGHT__");
}

void path_tracing::renderers::module_renderer::build_acceleration(const runtime_service& service)
{
	std::vector<wrapper::directx12::raytracing_instance> instances;

	for (const auto& entity : service.scene.entities) {
		if (!entity.mesh.has_value()) continue;
		
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

void path_tracing::renderers::module_renderer::build_shader_libraries(const runtime_service& service)
{
	auto ray_generation_submodule = wrapper::directx12::shader_creator::create_from_file(
		L"./resources/shaders/module_renderer/ray_generation.hlsl");

	ray_generation_submodule.replace("[material_submodule]", mMaterialSubmodule);
	ray_generation_submodule.replace("[light_submodule]", mLightSubmodule);
	ray_generation_submodule.replace("[macro_submodule]", mMacroSubmodule);

	std::vector<wrapper::directx12::shader_library> libraries = {
		wrapper::directx12::shader_library::create(
			wrapper::directx12::extensions::compile_from_source_using_dxc(ray_generation_submodule.source(),
			L"./resources/shaders/module_renderer/ray_generation.hlsl", L"", L"lib_6_4"),
			{ module_renderer_ray_generation }
		),
		wrapper::directx12::shader_library::create(
			wrapper::directx12::extensions::compile_from_file_using_dxc(
			L"./resources/shaders/module_renderer/closest_hits.hlsl", L"", L"lib_6_4"),
			{ module_renderer_closest_hit }
		),
		wrapper::directx12::shader_library::create(
			wrapper::directx12::extensions::compile_from_file_using_dxc(
			L"./resources/shaders/module_renderer/miss_shaders.hlsl", L"", L"lib_6_4"),
			{ module_renderer_miss_shader }
		)
	};
	
	mRaytracingPipelineInfo
		.set_shader_libraries(libraries)
		.set_ray_generation(module_renderer_ray_generation)
		.set_miss_shaders({ module_renderer_miss_shader });
}

void path_tracing::renderers::module_renderer::build_hit_groups(const runtime_service& service)
{
	std::vector<wrapper::directx12::raytracing_hit_group> hit_groups;

	for (size_t index = 0; index < service.scene.entities.size(); index++) {
		if (!service.scene.entities[index].mesh.has_value()) continue;

		// the index in the name of hit group is equal the entity index
		// but the index of hit_groups is not equal th entity index
		// because there are some entities that do not has meshes
		hit_groups.push_back({
			D3D12_HIT_GROUP_TYPE_TRIANGLES,
			L"", module_renderer_closest_hit, L"",
			L"hit_group_" + std::to_wstring(index)
			});
	}

	mRaytracingPipelineInfo.set_raytracing_hit_groups(hit_groups);
}

void path_tracing::renderers::module_renderer::build_structured_buffer(const runtime_service& service)
{
	std::vector<module_renderer_entity_info> entities;
	
	uint32 material_count = 0;
	uint32 light_count = 0;
	
	for (const auto& entity : service.scene.entities) {
		module_renderer_entity_info info;

		info.local_to_world = transpose(entity.transform.matrix());
		info.world_to_local = transpose(entity.transform.inverse().matrix());

		if (entity.mesh.has_value()) info.mesh = allocate_mesh_info(service, entity.transform, entity.mesh.value());
		if (entity.material.has_value()) info.material = material_count++;
		if (entity.light.has_value()) info.light = light_count++;

		entities.push_back(info);
	}

	auto materials = std::vector<byte>(mMaterialSubmoduleTypeSize * material_count);
	auto lights = std::vector<byte>(mLightSubmoduleTypeSize * light_count);

	material_count = 0;
	light_count = 0;

	for (const auto& entity : service.scene.entities) {
		if (entity.material.has_value()) {
			const auto target = materials.data() + material_count++ * mMaterialSubmoduleTypeSize;
			const auto type_index = mMaterialTypes.at(entity.material->submodule);

			copy_submodule_data_to(target, entity.material.value(), mMaterialValuesMemoryAddress);

			std::memcpy(target + mMaterialValuesMemoryAddress.at("type"), &type_index, sizeof(uint32));
		}

		if (entity.light.has_value()) {
			const auto target = lights.data() + light_count++ * mLightSubmoduleTypeSize;
			const auto type_index = mLightTypes.at(entity.light->submodule);
			
			copy_submodule_data_to(target, entity.light.value(), mLightValuesMemoryAddress);

			std::memcpy(target + mLightValuesMemoryAddress.at("type"), &type_index, sizeof(uint32));

			// record the environment index
			if (entity.light->submodule == "environment_light")
				mSceneConfig.environment = light_count - 1;
		}
	}

	// update light count
	mSceneConfig.lights = light_count;
	
	mEntityInfoCpuBuffer = wrapper::directx12::buffer::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::upload(),
		std::max(entities.size() * sizeof(module_renderer_entity_info), static_cast<size_t>(256))
	);

	mMaterialCpuBuffer = wrapper::directx12::buffer::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::upload(),
		std::max(materials.size(), static_cast<size_t>(256))
	);

	mLightCpuBuffer = wrapper::directx12::buffer::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::upload(),
		std::max(lights.size(), static_cast<size_t>(256))
	);
	
	mEntityInfoGpuBuffer = wrapper::directx12::buffer::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_COPY_DEST),
		mEntityInfoCpuBuffer.size_in_bytes()
	);

	mMaterialGpuBuffer = wrapper::directx12::buffer::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_COPY_DEST),
		mMaterialCpuBuffer.size_in_bytes()
	);

	mLightGpuBuffer = wrapper::directx12::buffer::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::common(D3D12_RESOURCE_STATE_COPY_DEST),
		mLightCpuBuffer.size_in_bytes()
	);

	mEntityInfoCpuBuffer.copy_from_cpu(entities.data(), entities.size() * sizeof(module_renderer_entity_info));
	mMaterialCpuBuffer.copy_from_cpu(materials.data(), materials.size());
	mLightCpuBuffer.copy_from_cpu(lights.data(), lights.size());

	mEntityInfoGpuBuffer.copy_from(mCommandList, mEntityInfoCpuBuffer);
	mMaterialGpuBuffer.copy_from(mCommandList, mMaterialCpuBuffer);
	mLightGpuBuffer.copy_from(mCommandList, mLightCpuBuffer);

	mEntityInfoGpuBuffer.barrier(mCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	mMaterialGpuBuffer.barrier(mCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
	mLightGpuBuffer.barrier(mCommandList, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void path_tracing::renderers::module_renderer::build_descriptor_heap(const runtime_service& service)
{
	mSceneConfigCpuBuffer = wrapper::directx12::buffer::create(
		service.render_device.device(),
		wrapper::directx12::resource_info::upload(),
		std::max(sizeof(module_renderer_scene_config), static_cast<size_t>(256)));

	mDescriptorTable
		.add_cbv_range({ "scene_config" }, 0, 0)
		.add_srv_range({ "acceleration" }, 0, 1)
		.add_uav_range({ "render_target_hdr", "render_target_sdr" }, 0, 2)
		.add_srv_range({ "entity_info", "lights", "materials" }, 0, 3)
		.add_srv_range({ "positions", "normals", "uvs", "indices" }, 0, 4)
		.add_srv_range({ "environment" }, 0, 5);
	
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

	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(sizeof(module_renderer_entity_info), mEntityInfoGpuBuffer.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("entity_info")),
		mEntityInfoGpuBuffer
	);

	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(mMaterialSubmoduleTypeSize, mMaterialGpuBuffer.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("materials")),
		mMaterialGpuBuffer
	);

	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(mLightSubmoduleTypeSize, mLightGpuBuffer.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("lights")),
		mLightGpuBuffer
	);
	
	const auto mesh_gpu_buffer = service.meshes_system.gpu_buffer();
	
	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(sizeof(vector3), mesh_gpu_buffer.positions.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("positions")),
		mesh_gpu_buffer.positions
	);

	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(sizeof(vector3), mesh_gpu_buffer.normals.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("normals")),
		mesh_gpu_buffer.normals
	);

	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(sizeof(vector3), mesh_gpu_buffer.uvs.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("uvs")),
		mesh_gpu_buffer.uvs
	);

	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(sizeof(unsigned) * 3, mesh_gpu_buffer.indices.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("indices")),
		mesh_gpu_buffer.indices
	);
}

void path_tracing::renderers::module_renderer::build_root_signature(const runtime_service& service)
{
	mRootSignatureInfo[0]
		.add_descriptor_table("descriptor_table", mDescriptorTable)
		.add_static_sampler("default_sampler", 0, 6);

	mRootSignatureInfo[1]
		.add_constants("hit_group_info", 0, 100, 1);
	
	mRootSignature[0] = wrapper::directx12::root_signature::create(service.render_device.device(), mRootSignatureInfo[0], false);
	mRootSignature[1] = wrapper::directx12::root_signature::create(service.render_device.device(), mRootSignatureInfo[1], true);

	mRaytracingPipelineInfo.set_root_signature(mRootSignature[0]);
}

void path_tracing::renderers::module_renderer::build_shader_association(const runtime_service& service)
{
	const wrapper::directx12::raytracing_shader_config config = {
		8,
		128
	};

	std::vector<wrapper::directx12::raytracing_shader_association> associations;

	// association of shaders(ray_generation, miss_shaders, closest_hit_shader)
	associations.push_back({ std::nullopt, config, module_renderer_ray_generation });
	associations.push_back({ std::nullopt, config, module_renderer_closest_hit });
	associations.push_back({ std::nullopt, config, module_renderer_miss_shader });

	for (const auto& hit_group : mRaytracingPipelineInfo.hit_groups()) {
		associations.push_back({
			wrapper::directx12::raytracing_shader_root_signature{
				mRootSignature[1], mRootSignatureInfo[1].size()
			}, std::nullopt, hit_group.name
			});
	}

	mRaytracingPipelineInfo.set_raytracing_shader_associations(associations);
}

void path_tracing::renderers::module_renderer::build_raytracing_pipeline(const runtime_service& service)
{
	mRaytracingPipeline = wrapper::directx12::raytracing_pipeline::create(service.render_device.device(), mRaytracingPipelineInfo);
}

void path_tracing::renderers::module_renderer::build_raytracing_shader_table(const runtime_service& service)
{
	std::vector<std::wstring> names;

	for (const auto& hit_group : mRaytracingPipelineInfo.hit_groups())
		names.push_back(hit_group.name);

	mRaytracingShaderTable = wrapper::directx12::raytracing_shader_table::create(
		mRaytracingPipelineInfo.associations(),
		{ module_renderer_miss_shader },
		names, module_renderer_ray_generation
	);

	for (const auto& record : mRaytracingShaderTable.records()) {
		std::memcpy(mRaytracingShaderTable.data() + record.second.address,
			mRaytracingPipeline.shader_identifier(record.first),
			D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	}

	for (uint32 index = 0; index < service.scene.entities.size(); index++) {
		if (!service.scene.entities[index].mesh.has_value()) continue;

		std::memcpy(mRaytracingShaderTable.shader_record_address(L"hit_group_" + std::to_wstring(index)) + 
			D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES,
			&index, sizeof(index));
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
