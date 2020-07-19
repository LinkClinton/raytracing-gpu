#include "import_mitsuba_emitter.hpp"

#include "../../emitters/surface_emitter.hpp"

#include "import_mitsuba_spectrum.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_area_emitter(const tinyxml2::XMLNode* node, std::shared_ptr<emitter>& emitter)
	{
		emitter = std::make_shared<surface_emitter>();

		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_SPECTRUM_ELEMENT)
					import_spectrum(current, std::static_pointer_cast<surface_emitter>(emitter)->radiance);

				if (current->Value() == MITSUBA_RGB_ELEMENT)
					import_rgb(current, std::static_pointer_cast<surface_emitter>(emitter)->radiance);
			});
	}

	void import_emitter(const tinyxml2::XMLNode* node, std::shared_ptr<emitter>& emitter)
	{
		const auto type = std::string(node->ToElement()->Attribute("type"));

		if (type == "area") import_area_emitter(node, emitter);
	}
}

#endif