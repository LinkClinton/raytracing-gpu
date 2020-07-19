#include "color_spectrum.hpp"

#include "sampled_spectrum.hpp"
#include "cie_samples.hpp"

#include <algorithm>
#include <vector>

namespace metascene::spectrums {

	inline real lerp(const real& s, const real& t, real x)
	{
		return s * (1 - x) + t * x;
	}

	inline real interpolate_spectrum_samples(
		const std::vector<real>& lambda,
		const std::vector<real>& value,
		real x)
	{
		if (x <= lambda.front()) return value.front();
		if (x >= lambda.back()) return value.back();

		const auto offset = static_cast<size_t>(std::lower_bound(lambda.begin(), lambda.end(), x) - lambda.begin());
		const auto t = (x - lambda[offset - 1]) / (lambda[offset] - lambda[offset - 1]);

		return lerp(value[offset - 1], value[offset], t);
	}

	inline std::shared_ptr<spectrum> create_color_spectrum_from_xyz(real x, real y, real z)
	{
		auto spectrum = std::make_shared<color_spectrum>();

		spectrum->red = +3.240479f * x - 1.537150f * y - 0.498535f * z;
		spectrum->green = -0.969256f * x + 1.875991f * y + 0.041556f * z;
		spectrum->blue = +0.055648f * x - 0.204043f * y + 1.057311f * z;

		return spectrum;
	}
}

metascene::spectrums::color_spectrum::color_spectrum() : spectrum(type::color)
{
}

metascene::spectrums::color_spectrum::color_spectrum(real red, real green, real blue) :
	spectrum(type::color), red(red), green(green), blue(blue)
{
}

metascene::spectrums::color_spectrum::color_spectrum(real value) :
	spectrum(type::color), red(value), green(value), blue(value)
{
}

std::string metascene::spectrums::color_spectrum::to_string() const noexcept
{
	std::stringstream stream;

	stream << "[color_spectrum] [red = " << red << ", green = " << green << ", blue = " << blue << "]";

	return stream.str();
}

std::shared_ptr<metascene::spectrums::spectrum> metascene::spectrums::create_color_spectrum_from_sampled(
	const std::shared_ptr<spectrum>& spectrum)
{
	if (spectrum->type != type::sampled) throw "sampled spectrum required.";

	auto sampled = std::static_pointer_cast<sampled_spectrum>(spectrum);
	
	real x = 0, y = 0, z = 0;

	for (size_t index = 0; index < cie_samples; index++) {
		const auto sampled_value = interpolate_spectrum_samples(sampled->lambda, sampled->value, cie_samples_lambda[index]);

		x = x + sampled_value * cie_samples_x[index];
		y = y + sampled_value * cie_samples_y[index];
		z = z + sampled_value * cie_samples_z[index];
	}

	const auto scale = (cie_samples_lambda[cie_samples - 1] - cie_samples_lambda[0]) / (cie_samples_y_integral * cie_samples);

	x = x * scale;
	y = y * scale;
	z = z * scale;

	return create_color_spectrum_from_xyz(x, y, z);
}
