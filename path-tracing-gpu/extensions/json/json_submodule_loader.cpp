#include "json_submodule_loader.hpp"

#include "../images/image_stb.hpp"
#include "../spdlog/spdlog.hpp"

#include <filesystem>

using namespace path_tracing::runtime::resources;

namespace path_tracing::extensions::json {

	identity get_identity_from_images_system(const resource_context& context, const std::string& filename, bool gamma)
	{
		if (filename.empty()) return index_null;
		
		// the coordinate is depend on scene config, so we do not need this property for cache
		// all images' coordinate in this scene are same.
		const auto image_cache_name = filename + "with gamma : " + std::to_string(gamma);
		
		if (context.images_system.has(image_cache_name)) return context.images_system.info(image_cache_name);

		return context.images_system.allocate(image_cache_name, images::read_image_sdr(
			context.directory + filename, context.coordinate, gamma));
	}

	void insert_submodule_texture_property(submodule_data& submodule, const resource_context& context, const nlohmann::json& material, const std::string& name)
	{
		submodule.texture.insert({ name, {
			material["properties"][name]["value"],
			get_identity_from_images_system(context, material["properties"][name]["image"], material["properties"][name]["gamma"])
		} });
	}
	
	submodule_data load_substrate_material_from_json(const resource_context& context, const nlohmann::json& material)
	{
		submodule_data submodule;

		insert_submodule_texture_property(submodule, context, material, "diffuse");
		insert_submodule_texture_property(submodule, context, material, "specular");
		insert_submodule_texture_property(submodule, context, material, "roughness");

		submodule.uint.insert({ "remapping", material["remapping"].get<bool>() ? 1 : 0 });
		
		submodule.submodule = "substrate_material";

		return submodule;
	}
	
	submodule_data load_diffuse_material_from_json(const resource_context& context, const nlohmann::json& material)
	{
		submodule_data submodule;

		insert_submodule_texture_property(submodule, context, material, "diffuse");
		
		submodule.submodule = "diffuse_material";

		return submodule;
	}

	submodule_data load_mirror_material_from_json(const resource_context& context, const nlohmann::json& material)
	{
		submodule_data submodule;

		insert_submodule_texture_property(submodule, context, material, "reflectance");
		
		submodule.submodule = "mirror_material";

		return submodule;
	}
	
	submodule_data load_glass_material_from_json(const resource_context& context, const nlohmann::json& material)
	{
		submodule_data submodule;
		
		insert_submodule_texture_property(submodule, context, material, "transmission");
		insert_submodule_texture_property(submodule, context, material, "reflectance");
		insert_submodule_texture_property(submodule, context, material, "eta");
		
		submodule.submodule = "glass_material";

		return submodule;
	}

	submodule_data load_metal_material_from_json(const resource_context& context, const nlohmann::json& material)
	{
		submodule_data submodule;

		insert_submodule_texture_property(submodule, context, material, "roughness");
		insert_submodule_texture_property(submodule, context, material, "eta");
		insert_submodule_texture_property(submodule, context, material, "k");
		
		submodule.uint.insert({ "remapping", material["remapping"].get<bool>() ? 1 : 0 });

		submodule.submodule = "metal_material";

		return submodule;
	}
	
	submodule_data load_material_from_json(const resource_context& context, const nlohmann::json& material)
	{
		if (material["type"] == "substrate") return load_substrate_material_from_json(context, material);
		if (material["type"] == "diffuse") return load_diffuse_material_from_json(context, material);
		if (material["type"] == "mirror") return load_mirror_material_from_json(context, material);
		if (material["type"] == "glass") return load_glass_material_from_json(context, material);
		if (material["type"] == "metal") return load_metal_material_from_json(context, material);

		spdlog::error("unknown material type : {0}.", material["type"].get<std::string>());
		
		return {};
	}

	submodule_data load_environment_light_from_json(const resource_context& context, const nlohmann::json& light, uint32 index)
	{
		submodule_data submodule;

		if (!light["environment"]["image"].get<std::string>().empty()) {
			auto filename = std::filesystem::path(light["environment"]["image"].get<std::string>());

			if (filename.extension() == ".exr") filename = filename.parent_path().string() + "/" + filename.stem().string() + ".hdr";
			
			context.images_system.allocate("environment", images::read_image_hdr(
				context.directory + filename.string(),
				coordinate_system::left_hand, light["environment"]["gamma"]));
		}
			
		submodule.float3.insert({ "intensity", light["intensity"] });

		submodule.uint.insert({ "entity", index });
		submodule.uint.insert({ "delta", 0 });

		submodule.submodule = "environment_light";

		return submodule;
	}

	submodule_data load_directional_light_from_json(const resource_context& context, const nlohmann::json& light, uint32 index)
	{
		submodule_data submodule;

		submodule.float3.insert({ "direction", light["directional"]["direction"] });
		submodule.float3.insert({ "intensity", light["intensity"] });
		
		submodule.uint.insert({ "entity", index });
		submodule.uint.insert({ "delta", 1 });

		submodule.submodule = "directional_light";
		
		return submodule;
	}
	
	submodule_data load_surface_light_from_json(const nlohmann::json& light, uint32 index)
	{	
		submodule_data submodule;

		submodule.float3.insert({ "intensity", light["intensity"] });

		submodule.uint.insert({ "entity", index });
		submodule.uint.insert({ "delta", 0 });
		
		submodule.submodule = "surface_light";
		
		return submodule;
	}
	
	submodule_data load_light_from_json(const resource_context& context, const nlohmann::json& light, uint32 index)
	{
		if (light["type"] == "environment") return load_environment_light_from_json(context, light, index);
		if (light["type"] == "directional") return load_directional_light_from_json(context, light, index);
		if (light["type"] == "surface") return load_surface_light_from_json(light, index);

		spdlog::error("unknown light type : {0}.", light["type"].get<std::string>());
		
		return {};
	}

}
