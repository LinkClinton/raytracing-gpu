#include "import_mitsuba_sensor.hpp"

#include "../../cameras/perspective_camera.hpp"
#include "../../samplers/random_sampler.hpp"
#include "../../filters/gaussian_filter.hpp"

#include "import_mitsuba_transform.hpp"

#ifdef __MITSUBA_IMPORTER__

namespace metascene::importers::mitsuba {

	void import_sampler(const tinyxml2::XMLNode* node, std::shared_ptr<sampler>& sampler)
	{
		sampler = std::make_shared<random_sampler>();

		// we do not process the sampler type
		// because the mitsuba 2.0 only support independent sampler now
		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				if (current->Value() == MITSUBA_INTEGER_ELEMENT) {
					const std::string name = current->ToElement()->Attribute("name");

					// if the name is "sampleCount"(mitsuba 0.6) or "sample_count"(mitsuba 2.0)
					// we will read the sample per pixel of scene
					if (name == "sampleCount" || name == "sample_count")
						import_integer(current, sampler->sample_per_pixel);
				}
			});
	}

	void import_filter(const tinyxml2::XMLNode* node, std::shared_ptr<filter>& filter)
	{
		const std::string type = node->ToElement()->Attribute("type");

		if (type == "gaussian") filter = std::make_shared<gaussian_filter>();
	}

	void import_film(const tinyxml2::XMLNode* node, std::shared_ptr<film>& film)
	{
		film = std::make_shared<cameras::film>();

		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				// when the current is integer value, it may be width or height
				if (current->Value() == MITSUBA_INTEGER_ELEMENT) {
					const std::string name = current->ToElement()->Attribute("name");

					if (name == "height") import_integer(current, film->height);
					if (name == "width") import_integer(current, film->width);
				}

				// if we find the filter value, it will be used to create film
				if (current->Value() == MITSUBA_FILTER_ELEMENT)
					import_filter(current, film->filter);
			});
	}
	
	void import_sensor(const tinyxml2::XMLNode* node, const std::shared_ptr<scene_cache>& cache)
	{
		const auto element = node->ToElement();

		// now we only support perspective camera.
		// so if there is no perspective camera, we do not process it
		if (element->Attribute("type") != std::string("perspective")) return;

		auto camera = cache->scene->camera = std::make_shared<perspective_camera>();

		loop_all_children(node, [&](const tinyxml2::XMLNode* current)
			{
				// if we find the transform element, it should be the transform from camera to world
				// so we process transform of camera
				if (current->Value() == MITSUBA_TRANSFORM_ELEMENT)
					import_transform(current, camera->transform);

				if (current->Value() == MITSUBA_SAMPLER_ELEMENT)
					import_sampler(current, cache->scene->sampler);

				// process the film element and create a film to info
				if (current->Value() == MITSUBA_FILM_ELEMENT)
					import_film(current, cache->scene->film);

				// if we find the float element and its name is fov
				// we will read a float as fov value(angle)
				if (current->Value() == MITSUBA_FLOAT_ELEMENT && current->ToElement()->Attribute("name") == std::string("fov"))
					import_float(current, std::static_pointer_cast<perspective_camera>(camera)->fov);
			});
	}
}

#endif