#include "sampled_spectrum.hpp"

#include "cie_samples.hpp"

#include <fstream>

metascene::spectrums::sampled_spectrum::sampled_spectrum() : spectrum(type::sampled)
{
}

std::string metascene::spectrums::sampled_spectrum::to_string() const noexcept
{
	std::stringstream stream;

	stream << "[sampled_spectrum] [";

	for (size_t index = 0; index < lambda.size(); index++) 
		stream << lambda[index] << " : " << value[index] << ", ";

	stream << "]";

	return stream.str();
}

std::shared_ptr<metascene::spectrums::sampled_spectrum> metascene::spectrums::read_sampled_spectrum_from_spd(const std::string& filename)
{
	std::fstream stream(filename);

	auto spectrum = std::make_shared<sampled_spectrum>();
	
	real lambda = 0, value = 0;

	while (stream >> lambda >> value) {
		spectrum->lambda.push_back(lambda);
		spectrum->value.push_back(value);
	}

	stream.close();

	return spectrum;
}

std::shared_ptr<metascene::spectrums::sampled_spectrum> metascene::spectrums::create_sampled_spectrum_from_black_body(
	const std::vector<real>& lambda, real temperature, real scale)
{
	auto spectrum = black_body(lambda, temperature);

	const auto lambda_max = static_cast<real>(2.8977721e-3 / temperature * 1e9);
	const auto max_L = black_body({ lambda_max }, temperature);

	for (size_t index = 0; index < spectrum->value.size(); index++) {
		spectrum->value[index] = spectrum->value[index] / max_L->value[0];
		spectrum->value[index] = spectrum->value[index] * scale;
	}

	return spectrum;
}

std::shared_ptr<metascene::spectrums::sampled_spectrum> metascene::spectrums::create_sampled_spectrum_from_black_body(
	real temperature, real scale)
{
	return create_sampled_spectrum_from_black_body(std::vector<real>(cie_samples_lambda, cie_samples_lambda + cie_samples),
		temperature, scale);
}

std::shared_ptr<metascene::spectrums::sampled_spectrum> metascene::spectrums::black_body(
	const std::vector<real>& lambda, real temperature)
{
	auto spectrum = std::make_shared<sampled_spectrum>();

	spectrum->lambda = lambda;
	spectrum->value = std::vector<real>(lambda.size(), 0);

	if (temperature <= 0) return spectrum;

	constexpr static auto c = static_cast<real>(299792458);
	constexpr static auto h = static_cast<real>(6.62606957e-34);
	constexpr static auto kb = static_cast<real>(1.3806488e-23);

	for (size_t index = 0; index < spectrum->value.size(); index++) {
		const auto lambda_pow_1 = static_cast<real>(lambda[index] * 1e-9);
		const auto lambda_pow_5 = (lambda_pow_1 * lambda_pow_1) * (lambda_pow_1 * lambda_pow_1) * lambda_pow_1;

		spectrum->value[index] = (2 * h * c * c) /
			(lambda_pow_5 * (exp((h * c) / (lambda_pow_1 * kb * temperature)) - 1));
	}

	return spectrum;
}

