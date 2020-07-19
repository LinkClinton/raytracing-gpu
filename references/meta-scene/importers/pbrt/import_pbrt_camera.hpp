#pragma once

#include "import_pbrt_include.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_film(scene_context& context, std::shared_ptr<film>& film);

	void import_sampler(scene_context& context, std::shared_ptr<sampler>& sampler);

	void import_filter(scene_context& context, std::shared_ptr<filter>& filter);
	
	void import_camera(scene_context& context, std::shared_ptr<camera>& camera, const matrix4x4& transform);
}

#endif