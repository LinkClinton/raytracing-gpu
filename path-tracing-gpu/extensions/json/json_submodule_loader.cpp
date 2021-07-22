#include "json_submodule_loader.hpp"

#include "../images/image_stb.hpp"
#include "../spdlog/spdlog.hpp"

using namespace path_tracing::runtime::resources;

namespace path_tracing::extensions::json {

	submodule_data load_substrate_material_from_json(const nlohmann::json& material)
	{
		submodule_data submodule;

		if (material["properties"]["diffuse"]["type"] == "constant")
			submodule.float3.insert({ "diffuse", material["properties"]["diffuse"]["constant"]["value"] });
		
		submodule.float3.insert({ "specular", material["properties"]["specular"]["constant"]["value"] });
		submodule.float3.insert({ "roughness", material["properties"]["roughness"]["constant"]["value"] });

		submodule.uint.insert({ "remapping", material["remapping"].get<bool>() ? 1 : 0 });
		
		submodule.submodule = "substrate_material";

		return submodule;
	}
	
	submodule_data load_diffuse_material_from_json(const nlohmann::json& material)
	{
		submodule_data submodule;

		if (material["properties"]["reflectance"]["type"] == "constant")
			submodule.float3.insert({ "diffuse", material["properties"]["reflectance"]["constant"]["value"] });

		submodule.submodule = "diffuse_material";

		return submodule;
	}

	submodule_data load_mirror_material_from_json(const nlohmann::json& material)
	{
		submodule_data submodule;

		if (material["properties"]["reflectance"]["type"] == "constant")
			submodule.float3.insert({ "reflectance", material["properties"]["reflectance"]["constant"]["value"] });

		submodule.submodule = "mirror_material";

		return submodule;
	}
	
	submodule_data load_glass_material_from_json(const nlohmann::json& material)
	{
		submodule_data submodule;

		submodule.float3.insert({ "transmission", material["properties"]["transmission"]["constant"]["value"] });
		submodule.float3.insert({ "reflectance", material["properties"]["reflectance"]["constant"]["value"] });
		submodule.float3.insert({ "eta", material["properties"]["eta"]["constant"]["value"] });

		submodule.submodule = "glass_material";

		return submodule;
	}

	submodule_data load_metal_material_from_json(const nlohmann::json& material)
	{
		submodule_data submodule;

		submodule.float3.insert({ "roughness", material["properties"]["roughness"]["constant"]["value"] });
		submodule.float3.insert({ "eta", material["properties"]["eta"]["constant"]["value"] });
		submodule.float3.insert({ "k", material["properties"]["k"]["constant"]["value"] });

		submodule.uint.insert({ "remapping", material["remapping"].get<bool>() ? 1 : 0 });

		submodule.submodule = "metal_material";

		return submodule;
	}
	
	submodule_data load_material_from_json(const nlohmann::json& material)
	{
		if (material["type"] == "substrate") return load_substrate_material_from_json(material);
		if (material["type"] == "diffuse") return load_diffuse_material_from_json(material);
		if (material["type"] == "mirror") return load_mirror_material_from_json(material);
		if (material["type"] == "glass") return load_glass_material_from_json(material);
		if (material["type"] == "metal") return load_metal_material_from_json(material);

		spdlog::error("unknown material type : {0}.", material["type"].get<std::string>());
		
		return {};
	}

	submodule_data load_environment_light_from_json(images_system& system, const nlohmann::json& light, 
		const std::string& directory, uint32 index)
	{
		submodule_data submodule;

		if (!light["environment"]["image"].get<std::string>().empty())
			system.allocate("environment", images::read_image_hdr(
				directory + light["environment"]["image"].get<std::string>(),
				coordinate_system::left_hand, light["environment"]["gamma"]));

		submodule.float3.insert({ "intensity", light["intensity"] });

		submodule.uint.insert({ "entity", index });
		submodule.uint.insert({ "delta", 0 });

		submodule.submodule = "environment_light";

		return submodule;
	}

	submodule_data load_directional_light_from_json(images_system& system, const nlohmann::json& light,
		const std::string& directory, uint32 index)
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
	
	submodule_data load_light_from_json(images_system& system, const nlohmann::json& light, const std::string& directory, uint32 index)
	{
		if (light["type"] == "environment") return load_environment_light_from_json(system, light, directory, index);
		if (light["type"] == "directional") return load_directional_light_from_json(system, light, directory, index);
		if (light["type"] == "surface") return load_surface_light_from_json(light, index);

		spdlog::error("unknown light type : {0}.", light["type"].get<std::string>());
		
		return {};
	}

}
