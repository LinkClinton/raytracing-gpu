#pragma once

#include "import_pbrt_include.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_medium_from_property_group(scene_context& context, const property_group& properties, std::shared_ptr<medium>& medium);

	void import_named_medium(scene_context& context);

	void import_named_medium(scene_context& context, std::shared_ptr<medium>& medium);

	void import_medium_interface(scene_context& context, std::shared_ptr<metascene::media::media>& media);
}

#endif