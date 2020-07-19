#pragma once

#include "import_mitsuba_include.hpp"

#include "../../spectrums/spectrum.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_spectrum(const tinyxml2::XMLNode* node, std::shared_ptr<spectrums::spectrum>& spectrum);

	void import_rgb(const tinyxml2::XMLNode* node, std::shared_ptr<spectrums::spectrum>& spectrum);
	
}

#endif