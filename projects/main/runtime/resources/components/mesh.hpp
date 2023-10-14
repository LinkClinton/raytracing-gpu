#pragma once

#include "../../../types.hpp"

namespace raytracing::runtime::resources::components
{
	struct mesh_info final
	{
		uint32 vtx_count = 0;
		uint32 idx_count = 0;
	};

	struct mesh_data final
	{
		std::vector<vector3> positions;
		std::vector<vector3> normals;
		std::vector<vector3> uvs;

		std::vector<uint32> indices;
	};
	
	struct mesh final
	{
		mesh_info info;
		mesh_data data;
	};
}
