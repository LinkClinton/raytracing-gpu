#pragma once

#include "../types.hpp"

namespace raytracing::runtime
{
	struct runtime_frame final
	{
		uint64 frame_index = 0;
		
		real delta_time = 0;
		real total_time = 0;
	};
	
}
