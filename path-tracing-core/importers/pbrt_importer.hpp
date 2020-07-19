#pragma once

#include "meta-scene/importers/pbrt/pbrt_importer.hpp"

#include "../renderer_scene.hpp"

#ifdef __PBRT_IMPORTER__

namespace path_tracing::core::importers {

	renderer_scene import_pbrt_scene(const std::string& filename);

}

#endif