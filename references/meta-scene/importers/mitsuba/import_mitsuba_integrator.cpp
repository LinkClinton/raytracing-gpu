#include "import_mitsuba_integrator.hpp"

#include "../../integrators/direct_integrator.hpp"
#include "../../integrators/path_integrator.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_direct_integrator(const tinyxml2::XMLNode* node, std::shared_ptr<integrator>& integrator)
	{
		integrator = std::make_shared<direct_integrator>();
		
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() != MITSUBA_INTEGER_ELEMENT) return;

				const auto name = std::string(current->ToElement()->Attribute("name"));

				if (name == "emitterSamples" || name == "emitter_samples")
					import_integer(current, std::static_pointer_cast<direct_integrator>(integrator)->emitter_samples);

				if (name == "bsdfSamples" || name == "bsdf_samples")
					import_integer(current, std::static_pointer_cast<direct_integrator>(integrator)->bsdf_samples);
			});
	}
	
	void import_path_integrator(const tinyxml2::XMLNode* node, std::shared_ptr<integrator>& integrator)
	{
		integrator = std::make_shared<path_integrator>();
	}

	void import_integrator(const tinyxml2::XMLNode* node, std::shared_ptr<integrator>& integrator)
	{
		const auto type = std::string(node->ToElement()->Attribute("type"));

		if (type == "direct") import_direct_integrator(node, integrator);
		if (type == "path") import_path_integrator(node, integrator);
	}
}

#endif