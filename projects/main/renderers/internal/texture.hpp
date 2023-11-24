#pragma once

#include "../../types.hpp"

namespace raytracing::renderers::internal
{
	struct texture final
	{
		vector3 value = vector3(1);

		uint32 index = index_null;
	};
}
