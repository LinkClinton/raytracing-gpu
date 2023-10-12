#pragma once

#include "../../types.hpp"

#include <string>

namespace raytracing::scenes::components
{
	struct material_property final
	{
		vector3 value = vector3(0);

		std::string image = "";
	};

	struct material final
	{
		std::string type = "unknown";

		mapping<std::string, material_property> properties;
	};

}
