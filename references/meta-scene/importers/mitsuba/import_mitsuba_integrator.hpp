#pragma once

#include "import_mitsuba_include.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_integrator(const tinyxml2::XMLNode* node, std::shared_ptr<integrator>& integrator);
	
}

#endif