#pragma once

#include "../../../types.hpp"

namespace raytracing::runtime::resources::components
{
	template <typename ResourceInfo, typename ResourceData>
	struct resource final
	{
		ResourceInfo info;
		ResourceData data;
	};
}
