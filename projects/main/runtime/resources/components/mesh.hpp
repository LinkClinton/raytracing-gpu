#pragma once

#include "resource.hpp"

namespace raytracing::runtime::resources::components
{
	enum class mesh_attribute : uint32
	{
		none = 0,
		position = 1 << 0,
		normal = 1 << 1,
		uv = 1 << 2,
		index = 1 << 3
	};

	inline void mark_enum_as_flag(mesh_attribute) {}
	
	struct mesh_info final
	{
		uint32 vtx_count = 0;
		uint32 idx_count = 0;

		mesh_attribute attribute = mesh_attribute::none;
	};
	
	struct mesh_data final
	{
		std::vector<vector3> positions;
		std::vector<vector3> normals;
		std::vector<vector3> uvs;

		std::vector<uint32> indices;
	};

	using mesh = resource<mesh_info, mesh_data>;
}
