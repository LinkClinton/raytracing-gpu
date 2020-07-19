#pragma once

#include "import_pbrt_include.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_integrator(scene_context& context, std::shared_ptr<integrator>& integrator);
	
}

#endif