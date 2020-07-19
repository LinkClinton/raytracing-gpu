#pragma once

#include "mitsuba_importer.hpp"

#include <functional>
#include <sstream>

#ifdef __MITSUBA_IMPORTER__

#include <tinyxml2.h>

namespace metascene::importers::mitsuba {

	const std::string MITSUBA_INTEGRATOR_ELEMENT = "integrator";
	const std::string MITSUBA_TRANSLATE_ELEMENT = "translate";
	const std::string MITSUBA_TRANSFORM_ELEMENT = "transform";
	const std::string MITSUBA_REFERENCE_ELEMENT = "ref";
	const std::string MITSUBA_SPECTRUM_ELEMENT = "spectrum";
	const std::string MITSUBA_SAMPLER_ELEMENT = "sampler";
	const std::string MITSUBA_INTEGER_ELEMENT = "integer";
	const std::string MITSUBA_EMITTER_ELEMENT = "emitter";
	const std::string MITSUBA_FILTER_ELEMENT = "rfilter";
	const std::string MITSUBA_SENSOR_ELEMENT = "sensor";
	const std::string MITSUBA_STRING_ELEMENT = "string";
	const std::string MITSUBA_SHAPE_ELEMENT = "shape";
	const std::string MITSUBA_POINT_ELEMENT = "point";
	const std::string MITSUBA_FLOAT_ELEMENT = "float";
	const std::string MITSUBA_FILM_ELEMENT = "film";
	const std::string MITSUBA_BSDF_ELEMENT = "bsdf";
	const std::string MITSUBA_RGB_ELEMENT = "rgb";

	vector3 string_to_vector3(const std::string& value);
	
	void loop_all_children(const tinyxml2::XMLNode* node, const std::function<void(const tinyxml2::XMLNode*)>& function);

	void import_float(const tinyxml2::XMLNode* node, real& value);

	template <typename T>
	void import_integer(const tinyxml2::XMLNode* node, T& value)
	{
		// <integer name = "integer_property" value = "1"/>
		value = string_to_integer<T>(node->ToElement()->Attribute("value"));
	}

	void import_point(const tinyxml2::XMLNode* node, vector3& point);

	struct scene_cache {
		std::unordered_map<std::string, std::shared_ptr<material>> materials;

		std::shared_ptr<scene> scene;

		std::string directory_path;

		scene_cache() = default;
	};
	
}

#endif