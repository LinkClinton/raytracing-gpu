#include "import_pbrt_spectrum.hpp"

#include "../../spectrums/sampled_spectrum.hpp"
#include "../../spectrums/color_spectrum.hpp"
#include "../../logs.hpp"

#ifdef __PBRT_IMPORTER__

using namespace metascene::spectrums;

namespace metascene::importers::pbrt {

	void import_color_spectrum(const std::string& token, std::shared_ptr<spectrum>& spectrum)
	{
		auto instance = std::make_shared<color_spectrum>();

		auto stream = std::stringstream(remove_special_character(token));

		stream >> instance->red >> instance->green >> instance->blue;

		spectrum = instance;
	}

	void import_sampled_spectrum(const std::string& filename, std::shared_ptr<spectrum>& spectrum)
	{
		spectrum = read_sampled_spectrum_from_spd(filename);
	}

	void import_black_body_spectrum(const std::string& token, std::shared_ptr<spectrums::spectrum>& spectrum)
	{
		auto stream = std::stringstream(remove_special_character(token));

		real temperature, scale;

		stream >> temperature >> scale;

		spectrum = create_sampled_spectrum_from_black_body(temperature, scale);
	}

	std::shared_ptr<spectrums::spectrum> multiply_spectrum(
		const std::shared_ptr<spectrums::spectrum>& lhs,
		const std::shared_ptr<spectrums::spectrum>& rhs)
	{
		const auto right_hand = rhs->type == spectrums::type::color ? rhs : create_color_spectrum_from_sampled(rhs);
		const auto left_hand = lhs->type == spectrums::type::color ? lhs : create_color_spectrum_from_sampled(lhs);

		return multiply_color_spectrum(left_hand, right_hand);
	}

	std::shared_ptr<spectrums::spectrum> multiply_color_spectrum(
		const std::shared_ptr<spectrums::spectrum>& lhs,
		const std::shared_ptr<spectrums::spectrum>& rhs)
	{
		const auto lhs_color = std::static_pointer_cast<color_spectrum>(lhs);
		const auto rhs_color = std::static_pointer_cast<color_spectrum>(rhs);
		
		return std::make_shared<color_spectrum>(
			lhs_color->red * rhs_color->red, 
			lhs_color->green * rhs_color->green,
			lhs_color->blue * rhs_color->blue);
	}
}

#endif