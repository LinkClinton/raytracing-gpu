#pragma once

#include "../../types.hpp"

#include <string>

namespace raytracing::renderers::internal
{
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
}