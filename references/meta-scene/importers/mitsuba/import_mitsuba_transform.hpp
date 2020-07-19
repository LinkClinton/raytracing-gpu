#pragma once

#include "import_mitsuba_include.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_transform(const tinyxml2::XMLNode* node, matrix4x4& transform);
	
}

#endif