#pragma once

#include "../types.hpp"

#include "components/transform.hpp"

#include <optional>
#include <string>

namespace raytracing::scenes
{
	struct texture final
	{
		vector3 value = vector3(1);
		std::string image = "";
	};

	struct submodule_data final
	{
		std::string type = "unknown";

		mapping<std::string, texture> textures;
	};

	struct submodule_mesh final
	{
		std::string name = "unknown";
	};
	
	struct entity final
	{
		std::optional<submodule_data> material;
		std::optional<submodule_data> light;
		std::optional<submodule_mesh> mesh;

		components::transform transform;
	};
}
