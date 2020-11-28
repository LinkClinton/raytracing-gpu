#include "module_renderer.hpp"

#include "directx12-wrapper/extensions/dxc.hpp"

namespace path_tracing::renderers {

	using scenes::components::submodule_data;
	
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
	build_shader_libraries(service);
	build_descriptor_heap(service);
	
	mCommandList.close();

	service.render_device.queue().execute({ mCommandList });
	service.render_device.wait();
}

void path_tracing::renderers::module_renderer::update(const runtime_service& service, const runtime_frame& frame)
{
}

void path_tracing::renderers::module_renderer::render(const runtime_service& service, const runtime_frame& frame)
{
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
		material_functions_sentences["evaluate"].add_sentence("case " + std::to_string(material.second) + ": evaluate_" + 
			material.first + "(material, wo, wi);", 1);
	}

	for (const auto& material : mMaterialTypes) {
		material_functions_sentences["sample"].add_sentence("case " + std::to_string(material.second) + ": sample_" +
			material.first + "(material, wo, value);", 1);
	}

	for (const auto& material : mMaterialTypes) {
		material_functions_sentences["pdf"].add_sentence("case " + std::to_string(material.second) + ": pdf_" +
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
		light_functions_sentences["evaluate"].add_sentence("case " + std::to_string(light.second) + ": evaluate_" +
			light.first + "(light, interaction, wi);", 1);
	}

	for (const auto& light : mLightTypes) {
		light_functions_sentences["sample"].add_sentence("case " + std::to_string(light.second) + ": sample_" +
			light.first + "(light, reference, value);", 1);
	}

	for (const auto& light : mLightTypes) {
		light_functions_sentences["pdf"].add_sentence("case " + std::to_string(light.second) + ": pdf_" +
			light.first + "(light, reference, wi);", 1);
	}
	
	mLightSubmodule.replace("[light]", common_light_struct);

	mLightSubmodule.replace("[include_light_submodule]", include_light_submodule);

	for (const auto& function_sentences : light_functions_sentences)
		mLightSubmodule.replace("[" + function_sentences.first + "_light]", function_sentences.second);
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

	std::vector<wrapper::directx12::shader_library> libraries = {
		wrapper::directx12::shader_library::create(
			wrapper::directx12::extensions::compile_from_source_using_dxc(ray_generation_submodule.source(),
				L"./resources/shaders/module_renderer/ray_generation.hlsl", L"", L"lib_6_4"),
			{ module_renderer_ray_generation }
		),
		wrapper::directx12::shader_library::create(
			wrapper::directx12::extensions::compile_from_file_using_dxc(
			L"./resources/shaders/depth_renderer/closest_hits.hlsl", L"", L"lib_6_4"),
			{ module_renderer_closest_hit }
		),
		wrapper::directx12::shader_library::create(
			wrapper::directx12::extensions::compile_from_file_using_dxc(
			L"./resources/shaders/depth_renderer/miss_shaders.hlsl", L"", L"lib_6_4"),
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
		hit_groups.push_back({
			D3D12_HIT_GROUP_TYPE_TRIANGLES,
			L"", module_renderer_closest_hit, L"",
			L"hit_group_" + std::to_wstring(index)
			});
	}

	mRaytracingPipelineInfo.set_raytracing_hit_groups(hit_groups);
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
		.add_srv_range({ "positions", "normals", "uvs", "indices" }, 0, 4);
	
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

	const auto mesh_gpu_buffer = service.meshes_system.gpu_buffer();
	
	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(sizeof(vector3), mesh_gpu_buffer.positions.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("positions"))
	);

	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(sizeof(vector3), mesh_gpu_buffer.normals.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("normals"))
	);

	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(sizeof(vector3), mesh_gpu_buffer.uvs.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("uvs"))
	);

	service.render_device.device().create_shader_resource_view(
		wrapper::directx12::resource_view::structured_buffer(sizeof(unsigned) * 3, mesh_gpu_buffer.indices.size_in_bytes()),
		mDescriptorHeap.cpu_handle(mDescriptorTable.index("indices"))
	);
}
