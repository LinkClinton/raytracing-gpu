#pragma once

#include "resource.hpp"

#include <directx12-wrapper/raytracings/accleration.hpp>

namespace raytracing::runtime::resources::components
{
	struct geometry_info final
	{
		uint32 vtx_location = 0;
		uint32 idx_location = 0;

		uint32 vtx_count = 0;
		uint32 idx_count = 0;
	};

	struct geometry_data final
	{
		wrapper::directx12::raytracing_geometry geometry;
	};

	using geometry = resource<geometry_info, geometry_data>;
}
