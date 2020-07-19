#include "mitsuba_importer.hpp"

#include "import_mitsuba_integrator.hpp"
#include "import_mitsuba_sensor.hpp"
#include "import_mitsuba_shape.hpp"
#include "import_mitsuba_bsdf.hpp"

#include <filesystem>

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_scene(const tinyxml2::XMLNode* node, const std::shared_ptr<scene_cache>& cache)
	{
		cache->scene = std::make_shared<scene>();

		std::shared_ptr<material> material;
		
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_INTEGRATOR_ELEMENT)
					import_integrator(current, cache->scene->integrator);

				if (current->Value() == MITSUBA_SENSOR_ELEMENT)
					import_sensor(current, cache);

				if (current->Value() == MITSUBA_BSDF_ELEMENT)
					import_bsdf(current, cache, material);

				if (current->Value() == MITSUBA_SHAPE_ELEMENT)
					import_shape(current, cache);
			});
	}

	std::shared_ptr<scene> import_mitsuba_scene(const std::string& filename)
	{
		const auto cache = std::make_shared<scene_cache>();

		cache->directory_path = std::filesystem::path(filename).parent_path().generic_string();

		tinyxml2::XMLDocument doc;

		doc.LoadFile(filename.c_str());

		import_scene(doc.LastChild(), cache);

		return cache->scene;
	}
}

#endif