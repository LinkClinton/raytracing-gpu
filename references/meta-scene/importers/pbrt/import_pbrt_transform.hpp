#pragma once

#include "import_pbrt_include.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_coord_sys_transform(scene_context& context, matrix4x4& transform);
	
	void import_look_at(scene_context& context, matrix4x4& transform);

	void import_translate(scene_context& context, matrix4x4& transform);
	
	void import_rotate(scene_context& context, matrix4x4& transform);

	void import_scale(scene_context& context, matrix4x4& transform);

	void import_matrix(scene_context& context, matrix4x4& transform);

	void import_concat_matrix(scene_context& context, matrix4x4& transform);
	
	void import_transform(scene_context& context);
}

#endif