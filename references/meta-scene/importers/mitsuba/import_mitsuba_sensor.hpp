#pragma once

#include "import_mitsuba_include.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_sensor(const tinyxml2::XMLNode* node, const std::shared_ptr<scene_cache>& cache);
	
}

#endif