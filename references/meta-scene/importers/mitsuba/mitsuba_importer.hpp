#pragma once

#include "../../utilities.hpp"
#include "../../scene.hpp"

#define __MITSUBA_IMPORTER__
#ifndef __MITSUBA_IMPORTER__
#else

namespace metascene::importers::mitsuba {

	std::shared_ptr<scene> import_mitsuba_scene(const std::string& filename);
	
}

#endif