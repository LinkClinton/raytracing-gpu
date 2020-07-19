#pragma once

#include "import_mitsuba_include.hpp"

#include "../../textures/texture.hpp"

#ifdef __MITSUBA_IMPORTER__

using namespace metascene::textures;

namespace metascene::importers::mitsuba {

	void import_spectrum_texture(const tinyxml2::XMLNode* node, std::shared_ptr<texture>& texture);

	void import_float_texture(const tinyxml2::XMLNode* node, std::shared_ptr<texture>& texture);
	
	void import_rgb_texture(const tinyxml2::XMLNode* node, std::shared_ptr<texture>& texture);
}

#endif