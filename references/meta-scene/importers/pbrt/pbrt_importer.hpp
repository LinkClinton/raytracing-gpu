#pragma once

#include "../../utilities.hpp"
#include "../../scene.hpp"

#define __PBRT_IMPORTER__
#ifndef __PBRT_IMPORTER__
#else

#include <string>

namespace metascene::importers::pbrt {

	std::shared_ptr<scene> import_pbrt_scene(const std::string& filename);

}

#endif