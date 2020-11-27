#include "module_renderer.hpp"

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

		for (const auto& value : data.texture) {
			assert(elements.find(value.first) != elements.end());

			if (elements.find(value.first + "_index") == elements.end())
				elements.insert({ value.first + "_index", "uint" });
		}
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

	const auto material_packing_layout = packing_submodule_data_layout(submodule_data_elements);

	mMaterialSubmodule.define_structure(material_packing_layout, "material");
	
	for (const auto& submodule : materials) {
		if (mMaterialTypes.find(submodule.name) == mMaterialTypes.end()) continue;

		mMaterialSubmodule.include(submodule.file);
	}
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

	std::vector<wrapper::directx12::shader_library> libraries = {
		
	};
}