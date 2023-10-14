#pragma once

#include "../types.hpp"

#include "components/transform.hpp"

#include <optional>
#include <string>

namespace raytracing::scenes
{
	struct submodule_data final
	{
		std::string type = "unknown";

		mapping<std::string, std::string> textures;
		mapping<std::string, vector3> value;
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
