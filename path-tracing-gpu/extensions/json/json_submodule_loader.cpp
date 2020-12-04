#include "json_submodule_loader.hpp"

namespace path_tracing::extensions::json {

	submodule_data load_diffuse_material_from_json(const nlohmann::json& material)
	{
		submodule_data submodule;

		submodule.float3.insert({ "diffuse", material["properties"]["reflectance"]["constant"]["value"] });

		submodule.submodule = "diffuse_material";

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
	
	submodule_data load_material_from_json(const nlohmann::json& material)
	{
		if (material["type"] == "diffuse") return load_diffuse_material_from_json(material);
		if (material["type"] == "glass") return load_glass_material_from_json(material);
		
		return {};
	}

	submodule_data load_environment_light_from_json(const nlohmann::json& light, uint32 index)
	{
		submodule_data submodule;

		submodule.float3.insert({ "intensity", light["intensity"] });

		submodule.uint.insert({ "entity", index });
		submodule.uint.insert({ "delta", 0 });

		submodule.submodule = "environment_light";

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
	
	submodule_data load_light_from_json(const nlohmann::json& light, uint32 index)
	{
		if (light["type"] == "environment") return load_environment_light_from_json(light, index);
		if (light["type"] == "surface") return load_surface_light_from_json(light, index);
		
		return {};
	}

}
