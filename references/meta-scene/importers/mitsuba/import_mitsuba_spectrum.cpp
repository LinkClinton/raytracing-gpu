#include "import_mitsuba_spectrum.hpp"

#include "../../spectrums/sampled_spectrum.hpp"
#include "../../spectrums/color_spectrum.hpp"

#ifdef __MITSUBA_IMPORTER__

using namespace metascene::spectrums;

namespace metascene::importers::mitsuba {

	std::shared_ptr<spectrum> string_to_sampled_spectrum(const std::string& value)
	{
		auto spectrum = std::make_shared<sampled_spectrum>();

		std::stringstream stream(value);

		std::string wave;

		while (std::getline(stream, wave, ',')) {
			const auto middle = wave.find(':');
			const auto lambda = string_to_real(wave.substr(0, middle));
			const auto strength = string_to_real(wave.substr(middle + 1, wave.length() - middle - 1));
			
			spectrum->lambda.push_back(lambda);
			spectrum->value.push_back(strength);
		}

		return spectrum;
	}

	std::shared_ptr<spectrum> string_to_color_spectrum(const std::string& value)
	{
		auto spectrum = std::make_shared<color_spectrum>();
		
		std::stringstream stream(value);
		std::string component[3];

		stream >> component[0] >> component[1] >> component[2];
		
		spectrum->red = string_to_real(component[0]);
		spectrum->green = string_to_real(component[1]);
		spectrum->blue = string_to_real(component[2]);

		return spectrum;
	}
	
	void import_sampled_spectrum(const tinyxml2::XMLNode* node, std::shared_ptr<spectrum>& spectrum)
	{
		/*
		 * <spectrum name = "reflectance" value = "length:value, length:value..."/>
		 */
		spectrum = string_to_sampled_spectrum(node->ToElement()->Attribute("value"));
	}

	void import_simple_spectrum(const tinyxml2::XMLNode* node, std::shared_ptr<spectrum>& spectrum)
	{
		const auto value = std::string(node->ToElement()->Attribute("value"));
		
		spectrum = std::make_shared<color_spectrum>();

		std::static_pointer_cast<color_spectrum>(spectrum)->red = string_to_real(value);
		std::static_pointer_cast<color_spectrum>(spectrum)->green = string_to_real(value);
		std::static_pointer_cast<color_spectrum>(spectrum)->blue = string_to_real(value);
	}

	void import_color_spectrum(const tinyxml2::XMLNode* node, std::shared_ptr<spectrum>& spectrum)
	{
		/*
		 * 	<rgb name="" value="red, blue, green"/>
		 */
		spectrum = string_to_color_spectrum(node->ToElement()->Attribute("value"));
	}
	
	void import_spectrum(const tinyxml2::XMLNode* node, std::shared_ptr<spectrum>& spectrum)
	{
		const auto value = std::string(node->ToElement()->Attribute("value"));

		if (value.find(':') != std::string::npos)
			import_sampled_spectrum(node, spectrum);
		else
			import_simple_spectrum(node, spectrum);
	}

	void import_rgb(const tinyxml2::XMLNode* node, std::shared_ptr<spectrum>& spectrum)
	{
		import_color_spectrum(node, spectrum);
	}
}

#endif