#include "material.hpp"
#include "texture.hpp"

static std::vector<raytracing::renderers::internal::material_metadata> metadata = 
{
	raytracing::renderers::internal::material_metadata
	{
		"diffuse",
		std::vector<std::string>
		{
			"diffuse"
		}
	}
};

const raytracing::renderers::internal::material_metadata& raytracing::renderers::internal::material_metadata::get_by_identity(
	uint32 identity)
{
	static mapping<uint32, uint32> material_identity_table;

	if (material_identity_table.empty())
	{
		for (size_t index = 0; index < metadata.size(); index++)
		{
			metadata[index].identity = static_cast<uint32>(index);
			metadata[index].size = static_cast<uint32>(metadata[index].textures.size() * sizeof(texture));

			material_identity_table.insert({ metadata[index].identity, metadata[index].identity });
		}
	}

	return metadata[material_identity_table.at(identity)];
}

const raytracing::renderers::internal::material_metadata& raytracing::renderers::internal::material_metadata::get_by_name(
	const std::string& name)
{
	static mapping<std::string, uint32> material_name_table;

	if (material_name_table.empty())
	{
		for (size_t index = 0; index < metadata.size(); index++)
		{
			material_name_table.insert({ metadata[index].name, metadata[index].identity });
		}
	}

	return get_by_identity(material_name_table.at(name));
}

raytracing::uint32 raytracing::renderers::internal::material_metadata::count()
{
	return static_cast<uint32>(metadata.size());
}