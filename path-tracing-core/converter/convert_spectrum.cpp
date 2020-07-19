#include "convert_spectrum.hpp"

#include "meta-scene/spectrums/sampled_spectrum.hpp"
#include "meta-scene/spectrums/color_spectrum.hpp"

namespace path_tracing::core::converter {

	vector3 read_sampled_spectrum(const std::shared_ptr<metascene::spectrums::sampled_spectrum>& spectrum)
	{
		const std::shared_ptr<metascene::spectrums::color_spectrum> color =
			std::static_pointer_cast<metascene::spectrums::color_spectrum>(metascene::spectrums::create_color_spectrum_from_sampled(spectrum));

		return vector3(color->red, color->green, color->blue);
	}

	vector3 read_color_spectrum(const std::shared_ptr<metascene::spectrums::color_spectrum>& spectrum)
	{
		return vector3(spectrum->red, spectrum->green, spectrum->blue);
	}
	
	vector3 read_spectrum(const std::shared_ptr<metascene::spectrums::spectrum>& spectrum)
	{
		if (spectrum->type == metascene::spectrums::type::color)
			return read_color_spectrum(std::static_pointer_cast<metascene::spectrums::color_spectrum>(spectrum));

		if (spectrum->type == metascene::spectrums::type::sampled)
			return read_sampled_spectrum(std::static_pointer_cast<metascene::spectrums::sampled_spectrum>(spectrum));

		return vector3(0);
	}
}
