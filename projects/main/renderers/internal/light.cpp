#include "light.hpp"
#include "texture.hpp"

static std::vector<raytracing::renderers::internal::light_metadata> metadata =
{
	raytracing::renderers::internal::light_metadata
	{
		"surface",
		std::vector<std::string>
		{
			"intensity"
		}
	},
	raytracing::renderers::internal::light_metadata
	{
		"environment",
		std::vector<std::string>
		{
			"intensity"
		}
	}
};

const raytracing::renderers::internal::light_metadata& raytracing::renderers::internal::light_metadata::get_by_identity(
	uint32 identity)
{
	static mapping<uint32, uint32> light_identity_table;

	if (light_identity_table.empty())
	{
		for (size_t index = 0; index < metadata.size(); index++)
		{
			metadata[index].identity = static_cast<uint32>(index);
			metadata[index].size = static_cast<uint32>(metadata[index].textures.size() * sizeof(texture));

			light_identity_table.insert({ metadata[index].identity, metadata[index].identity });
		}
	}

	return metadata[light_identity_table.at(identity)];
}

const raytracing::renderers::internal::light_metadata& raytracing::renderers::internal::light_metadata::get_by_name(
	const std::string& name)
{
	static mapping<std::string, uint32> light_name_table;

	if (light_name_table.empty())
	{
		for (size_t index = 0; index < metadata.size(); index++)
		{
			light_name_table.insert({ metadata[index].name, metadata[index].identity });
		}
	}

	return get_by_identity(light_name_table.at(name));
}

raytracing::uint32 raytracing::renderers::internal::light_metadata::count()
{
	return static_cast<uint32>(metadata.size());
}
