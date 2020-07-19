#pragma once

#include "import_pbrt_include.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_light_source(scene_context& context);

	void import_area_light_source(scene_context& context, std::shared_ptr<emitter>& emitter);
}

#endif