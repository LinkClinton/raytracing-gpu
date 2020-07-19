#pragma once

#include "import_pbrt_include.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_material_from_property_group(scene_context& context, const property_group& properties, std::shared_ptr<material>& material);
	
	void import_material(scene_context& context, std::shared_ptr<material>& material);

	void import_named_material(scene_context& context);

	void import_named_material(scene_context& context, std::shared_ptr<material>& material);
}

#endif