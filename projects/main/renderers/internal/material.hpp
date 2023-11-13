#pragma once

#include "../../scenes/entity.hpp"
#include "../../types.hpp"

namespace raytracing::renderer::internal
{
	enum class material_type : uint32
	{
		diffuse = 0,
		none = index_null,
	};

	struct texture final
	{
		vector3 value = vector3(1);

		uint32 index = index_null;
	};

	struct diffuse_material final
	{
		texture diffuse;
	};

	inline texture read_property_from_material(
		const mapping<std::string, uint32>& texture_indexer,
		const scenes::submodule_data& material,
		const std::string& property)
	{
		const texture result =
		{
			.value = material.value.at(property),
			.index = material.textures.at(property).empty() ? index_null : texture_indexer.at(material.textures.at(property))
		};

		return result;
	}
}