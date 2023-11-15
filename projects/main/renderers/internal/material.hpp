#pragma once

#include "../../scenes/entity.hpp"
#include "../../types.hpp"

namespace raytracing::renderers::internal
{
	struct texture final
	{
		vector3 value = vector3(1);

		uint32 index = index_null;
	};

	/*
	 * material is the array of textures
	 * each texture contain value(vector3) and index(uint, used to look up texture)
	 * material identity should be continuous for each type material
	 */
	struct material_metadata final
	{
		std::string name = "unknown";
		std::vector<std::string> textures;

		uint32 identity = 0;
		uint32 size = 0;

		static const material_metadata& get_by_identity(uint32 identity);
		static const material_metadata& get_by_name(const std::string& name);
		static uint32 count();
	};

	texture read_property_from_material(
		const mapping<std::string, uint32>& texture_indexer,
		const scenes::submodule_data& material,
		const std::string& property);
}