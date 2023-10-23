#pragma once

#include "resource.hpp"

#include <directx12-wrapper/raytracings/accleration.hpp>
#include <directx12-wrapper/resources/buffer.hpp>

namespace raytracing::runtime::resources::components
{
	struct mesh_info final
	{
		uint32 vtx_count = 0;
		uint32 idx_count = 0;
	};

	struct cpu_mesh_data final
	{
		std::vector<vector3> positions;
		std::vector<vector3> normals;
		std::vector<vector3> uvs;

		std::vector<uint32> indices;
	};
	
	struct gpu_mesh_data final
	{
		wrapper::directx12::buffer positions;
		wrapper::directx12::buffer normals;
		wrapper::directx12::buffer uvs;

		wrapper::directx12::buffer indices;
	};

	using cpu_mesh = resource<mesh_info, cpu_mesh_data>;
	using gpu_mesh = resource<mesh_info, gpu_mesh_data>;
}
