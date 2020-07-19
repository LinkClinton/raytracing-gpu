#include "import_mitsuba_texture.hpp"

#include "import_mitsuba_spectrum.hpp"

#include "../../spectrums/sampled_spectrum.hpp"
#include "../../textures/constant_texture.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_spectrum_texture(const tinyxml2::XMLNode* node, std::shared_ptr<texture>& texture)
	{
		auto instance = std::make_shared<constant_texture>(value_type::spectrum);

		import_spectrum(node, instance->spectrum);

		texture = instance;
	}

	void import_float_texture(const tinyxml2::XMLNode* node, std::shared_ptr<texture>& texture)
	{
		auto instance = std::make_shared<constant_texture>(value_type::real);

		import_float(node, instance->real);

		texture = instance;
	}

	void import_rgb_texture(const tinyxml2::XMLNode* node, std::shared_ptr<texture>& texture)
	{
		auto instance = std::make_shared<constant_texture>(value_type::spectrum);

		import_rgb(node, instance->spectrum);

		texture = instance;
	}

}

#endif