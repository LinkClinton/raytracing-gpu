#include "pbrt_importer.hpp"

#ifdef __PBRT_IMPORTER__

path_tracing::core::renderer_scene path_tracing::core::importers::import_pbrt_scene(const std::string& filename)
{
	const auto meta_scene = metascene::importers::pbrt::import_pbrt_scene(filename);

	return create_from_meta_scene(meta_scene);
}

#endif