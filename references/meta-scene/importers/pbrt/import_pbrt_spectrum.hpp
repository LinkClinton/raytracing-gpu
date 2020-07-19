#pragma once

#include "import_pbrt_include.hpp"

#include "../../spectrums/spectrum.hpp"

#ifdef __PBRT_IMPORTER__

namespace metascene::importers::pbrt {

	void import_color_spectrum(const std::string& token, std::shared_ptr<spectrums::spectrum>& spectrum);
	
	void import_sampled_spectrum(const std::string& filename, std::shared_ptr<spectrums::spectrum>& spectrum);

	void import_black_body_spectrum(const std::string& token, std::shared_ptr<spectrums::spectrum>& spectrum);

	std::shared_ptr<spectrums::spectrum> multiply_spectrum(
		const std::shared_ptr<spectrums::spectrum>& lhs,
		const std::shared_ptr<spectrums::spectrum>& rhs);
	
	std::shared_ptr<spectrums::spectrum> multiply_color_spectrum(
		const std::shared_ptr<spectrums::spectrum>& lhs,
		const std::shared_ptr<spectrums::spectrum>& rhs);
}

#endif