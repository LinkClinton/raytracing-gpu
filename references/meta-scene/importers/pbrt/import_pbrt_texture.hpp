#pragma once

#include "import_pbrt_include.hpp"

#include "../../textures/texture.hpp"

#ifdef __PBRT_IMPORTER__

using namespace metascene::textures;

namespace metascene::importers::pbrt {

	void import_color_spectrum_texture(const std::string& token, std::shared_ptr<texture>& texture);

	void import_sampled_spectrum_texture(const std::string& filename, std::shared_ptr<texture>& texture);
	
	void import_real_texture(const std::string& token, std::shared_ptr<texture>& texture);

	void import_texture(scene_context& context);
}

#endif